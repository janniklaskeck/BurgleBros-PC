// Fill out your copyright notice in the Description page of Project Settings.

#include "BBPlayerCameraManager.h"
#include "BBUtilities.h"
#include "BBPlayerController.h"
#include "BBMainGameMode.h"
#include "BBPlayerCharacterBase.h"
#include "BBGuard.h"
#include "BBLevel.h"
#include "BBRoomBase.h"
#include "BBSplineDrawer.h"

void ABBPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
	CurrentZoom = MaxCameraDistance;
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	GameMode->DOnEnterPlayerTurn.AddDynamic(this, &ABBPlayerCameraManager::OnEnterPlayerOrGuardTurn);
	GameMode->DOnEnterGuardTurn.AddDynamic(this, &ABBPlayerCameraManager::OnEnterPlayerOrGuardTurn);
}

void ABBPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
	if (CurrentPosition == FVector::ZeroVector && GetCurrentViewTarget())
	{
		CurrentPosition = GetCurrentViewTarget()->GetActorLocation();
	}
	FTViewTarget NewViewTarget;
	NewViewTarget.SetNewTarget(GetCurrentViewTarget());
	NewViewTarget.POV = GetPOV(DeltaTime);
	OutVT = NewViewTarget;
}

FMinimalViewInfo ABBPlayerCameraManager::GetPOV(float DeltaTime)
{
	FMinimalViewInfo ViewInfo;
	ViewInfo.FOV = DefaultFOVValue;
	ViewInfo.Rotation = GetDefaultRotation();
	ViewInfo.Location = GetCameraLocation(DeltaTime);
	ViewInfo.ProjectionMode = ECameraProjectionMode::Perspective;
	ViewInfo.PostProcessSettings = Settings;
	ViewInfo.PostProcessBlendWeight = 1.0f;
	return ViewInfo;
}

AActor* ABBPlayerCameraManager::GetCurrentViewTarget()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (GameMode->IsGuardTurn())
	{
		return GameMode->GetCurrentGuardPawn();
	}
	else
	{
		return GameMode->GetCurrentPlayerCharacter();
	}
}

FVector ABBPlayerCameraManager::GetCameraLocation(float DeltaTime)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (GameMode->IsGuardTurn())
	{
		CurrentTarget = GetCurrentViewTarget()->GetActorLocation();
	}
	CurrentTarget.Z = CurrentFloor * GameMode->GetCurrentLevel()->FloorHeight;
	CurrentPosition = FMath::Lerp(CurrentPosition, CurrentTarget, LerpSpeed * DeltaTime);
	FVector TargetLocation = CurrentPosition - GetDefaultRotation().Vector() * CurrentZoom;
	return TargetLocation;
}

void ABBPlayerCameraManager::FocusOnViewTarget()
{
	ABBCharacterBase* CurrentCharacter = Cast<ABBCharacterBase>(GetCurrentViewTarget());
	CurrentTarget = CurrentCharacter->GetActorLocation();
	CurrentFloor = CurrentCharacter->CurrentRoom->Position.Floor;
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	for (int32 Index = CurrentFloor + 1; Index < GameMode->GetCurrentLevel()->AmountFloors; Index++)
	{
		GameMode->GetCurrentLevel()->GetFloor(Index).SetFloorHidden(true);
	}
	for (int32 Index = 0; Index < CurrentFloor + 1; Index++)
	{
		GameMode->GetCurrentLevel()->GetFloor(Index).SetFloorHidden(false);
	}
}

void ABBPlayerCameraManager::SetCustomViewtarget(FVector TargetLocation, int32 TargetFloor)
{
	CurrentTarget = TargetLocation;
	MoveToFloor(TargetFloor);
}

void ABBPlayerCameraManager::OnEnterPlayerOrGuardTurn()
{
	FocusOnViewTarget();
}

void ABBPlayerCameraManager::MoveToFloor(int32 Floor)
{
	for (int32 Index = 0; Index < FMath::Abs(CurrentFloor - Floor); Index++)
	{
		if (CurrentFloor < Floor)
		{
			MoveUpFloor();
		}
		else if(CurrentFloor > Floor)
		{
			MoveDownFloor();
		}
	}
}

void ABBPlayerCameraManager::Zoom(float AxisValue)
{
	CurrentZoom -= ZoomSpeed * AxisValue;
	CurrentZoom = FMath::Clamp(CurrentZoom, MinCameraDistance, MaxCameraDistance);
}

void ABBPlayerCameraManager::MoveForward(float AxisValue)
{
	float CurrentVertical = CurrentTarget.X;
	float TargetVertical = CurrentVertical + MoveSpeed * AxisValue;
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	float MaxVertical = (GameMode->GetCurrentLevel()->FloorWidth - 1) * GameMode->GetCurrentLevel()->TileWidth;
	CurrentTarget.X = FMath::Clamp(TargetVertical, 0.f, MaxVertical);
}

void ABBPlayerCameraManager::MoveRight(float AxisValue)
{
	float CurrentHorizontal = CurrentTarget.Y;
	float TargetHorizontal = CurrentHorizontal + MoveSpeed * AxisValue;
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	float MaxHorizontal = (GameMode->GetCurrentLevel()->FloorWidth - 1) * GameMode->GetCurrentLevel()->TileWidth;
	CurrentTarget.Y = FMath::Clamp(TargetHorizontal, 0.f, MaxHorizontal);
}

void ABBPlayerCameraManager::MoveUpFloor()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (CurrentFloor < GameMode->GetCurrentLevel()->AmountFloors - 1)
	{
		++CurrentFloor;
		GameMode->GetCurrentLevel()->GetFloor(CurrentFloor).SetFloorHidden(false);
	}
}

void ABBPlayerCameraManager::MoveDownFloor()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (CurrentFloor > 0)
	{
		GameMode->GetCurrentLevel()->GetFloor(CurrentFloor).SetFloorHidden(true);
		--CurrentFloor;
		GameMode->SplineDrawer->DrawGuardPath(CurrentFloor);
	}
}
