// Fill out your copyright notice in the Description page of Project Settings.

#include "BBSplineDrawer.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "BBLevel.h"
#include "BBRoomBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BBGuard.h"

ABBSplineDrawer::ABBSplineDrawer()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	GuardReachableIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>("ReachableIndicatorMesh");
	GuardReachableIndicatorMesh->SetupAttachment(RootComponent);
	GuardFinalIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>("FinalIndicatorMesh");
	GuardFinalIndicatorMesh->SetupAttachment(RootComponent);
	PlayerReachIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlayerReachIndicatorMesh");
	PlayerReachIndicatorMesh->SetupAttachment(RootComponent);
}

void ABBSplineDrawer::BeginPlay()
{
	Super::BeginPlay();
	GuardFinalIndicatorMesh->SetVisibility(false);
	GuardReachableIndicatorMesh->SetVisibility(false);
	PlayerReachIndicatorMesh->SetVisibility(false);
}

void ABBSplineDrawer::DrawPath(TArray<FRoomPosition> Path, class USplineComponent* SplineComponent, TArray<USplineMeshComponent*>& SplineMeshComponents, UMaterialInterface* Material)
{
	if (Path.Num() <= 1)
	{
		return;
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	ABBLevel* CurrentLevel = GameMode->GetCurrentLevel();

	ClearPath(SplineComponent, SplineMeshComponents);
	for (FRoomPosition RoomPosition : Path)
	{
		ABBRoomBase* Room = CurrentLevel->GetRoomAt(RoomPosition);
		SplineComponent->AddSplinePoint(Room->GetActorLocation() + SplineOffset, ESplineCoordinateSpace::World);
	}
	UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	int32 SplinePoints = SplineComponent->GetNumberOfSplinePoints();
	for (int32 Index = 1; Index < SplinePoints; Index++)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, NAME_None);
		SplineMeshComponent->RegisterComponent();
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		SplineMeshComponents.Add(SplineMeshComponent);

		SplineMeshComponent->SetMaterial(0, MaterialInstance);
		SplineMeshComponent->SetStaticMesh(SplineStaticMesh);

		FVector StartLocation;
		FVector StartTangent;
		SplineComponent->GetLocationAndTangentAtSplinePoint(Index - 1, StartLocation, StartTangent, ESplineCoordinateSpace::World);

		FVector EndLocation;
		FVector EndTangent;
		SplineComponent->GetLocationAndTangentAtSplinePoint(Index, EndLocation, EndTangent, ESplineCoordinateSpace::World);

		SplineMeshComponent->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
	}
}

void ABBSplineDrawer::DrawGuardPath(int32 Floor)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	ABBGuard* Guard = GameMode->GetGuard(Floor);
	if (!Guard)
		return;
	TArray<FRoomPosition> Path;
	Path.Add(Guard->CurrentRoom->Position);
	Path.Append(Guard->GetPath(Guard->CurrentPatrolInfo.TargetRoom));
	for (int32 Index = 0; Index < Floor + 1 - GuardSplineComponents.Num(); Index++)
	{
		USplineComponent* GuardSplineComponent = NewObject<USplineComponent>(this, NAME_None);
		GuardSplineComponent->RegisterComponent();
		GuardSplineComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		GuardSplineComponents.Add(GuardSplineComponent);
		GuardSplineMeshComponents.Add(TArray<USplineMeshComponent*>());
	}
	DrawPath(Path, GuardSplineComponents[Floor], GuardSplineMeshComponents[Floor], GuardMeshMaterial);

	GuardFinalIndicatorMesh->SetVisibility(false);
	GuardReachableIndicatorMesh->SetVisibility(false);

	int32 ReachableIndex = Guard->Actions.GetCurrentActions() == 0 ? Guard->Actions.GetMaxActions() : Guard->Actions.GetCurrentActions();
	FRoomPosition ReachableRoom = Path[FMath::Min(ReachableIndex, Path.Num() - 1)];
	if (Path[Path.Num() - 1].IsSameRoom(ReachableRoom))
	{
		GuardFinalIndicatorMesh->SetWorldLocation(GameMode->GetCurrentLevel()->GetRoomAt(ReachableRoom)->GetActorLocation() + IndicatorOffset);
		GuardFinalIndicatorMesh->SetVisibility(true);
		GuardReachableIndicatorMesh->SetVisibility(false);
	}
	else
	{
		GuardFinalIndicatorMesh->SetWorldLocation(GameMode->GetCurrentLevel()->GetRoomAt(Path[Path.Num() - 1])->GetActorLocation() + IndicatorOffset);
		GuardReachableIndicatorMesh->SetWorldLocation(GameMode->GetCurrentLevel()->GetRoomAt(ReachableRoom)->GetActorLocation() + IndicatorOffset);
		GuardFinalIndicatorMesh->SetVisibility(true);
		GuardReachableIndicatorMesh->SetVisibility(true);
	}
	if (Path[0].IsSameRoom(ReachableRoom))
	{
		GuardReachableIndicatorMesh->SetVisibility(false);
	}
}

void ABBSplineDrawer::DrawPlayerPath()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	ABBPlayerCharacterBase* CurrentPlayer = GameMode->GetCurrentPlayerCharacter();
	if (!GameMode->GetSelectedRoom() || !CurrentPlayer)
	{
		ClearPlayerPath();
		return;
	}
	if (!PlayerSplineComponent)
	{
		PlayerSplineComponent = NewObject<USplineComponent>(this, "PlayerSpineComponent");
		PlayerSplineComponent->RegisterComponent();
		PlayerSplineComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
	}
	TArray<FRoomPosition> Path;
	TArray<FRoomPosition> PathToTarget = CurrentPlayer->GetPath(GameMode->GetSelectedRoom()->Position);
	if (PathToTarget.Num() == 0 || CurrentPlayer->Actions.GetCurrentActions() == 0)
	{
		ClearPlayerPath();
		return;
	}
	Path.Add(CurrentPlayer->CurrentRoom->Position);
	int32 MaxReachableIndex = FMath::Min(CurrentPlayer->Actions.GetCurrentActions(), PathToTarget.Num());
	for (int32 Index = 0; Index < MaxReachableIndex; Index++)
	{
		Path.Add(PathToTarget[Index]);
	}
	DrawPath(Path, PlayerSplineComponent, PlayerSplineMeshComponents, PlayerMeshMaterial);
	PlayerReachIndicatorMesh->SetWorldLocation(GameMode->GetCurrentLevel()->GetRoomAt(Path[Path.Num() - 1])->GetActorLocation() + IndicatorOffset);
	PlayerReachIndicatorMesh->SetVisibility(true);
}

void ABBSplineDrawer::ClearPath(class USplineComponent* SplineComponent, TArray<USplineMeshComponent*>& SplineMeshComponents)
{
	if (SplineComponent)
		SplineComponent->ClearSplinePoints(true);
	for (USplineMeshComponent* MeshComponent : SplineMeshComponents)
	{
		MeshComponent->DestroyComponent();
	}
	SplineMeshComponents.Reset(8);
}

void ABBSplineDrawer::ClearGuardPath(int32 Floor)
{
	if (GuardSplineComponents.Num() > 0 && Floor < GuardSplineComponents.Num())
	{
		ClearPath(GuardSplineComponents[Floor], GuardSplineMeshComponents[Floor]);
		GuardReachableIndicatorMesh->SetVisibility(false);
		GuardFinalIndicatorMesh->SetVisibility(false);
	}
}

void ABBSplineDrawer::ClearPlayerPath()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	ClearPath(PlayerSplineComponent, PlayerSplineMeshComponents);
	PlayerReachIndicatorMesh->SetVisibility(false);
}
