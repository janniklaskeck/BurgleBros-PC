// Fill out your copyright notice in the Description page of Project Settings.

#include "BBCharacterBase.h"
#include "BBLevel.h"
#include "BBGuard.h"
#include "BBLevelNavigationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

ABBCharacterBase::ABBCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	LevelNavigationComponent = CreateDefaultSubobject<UBBLevelNavigationComponent>("LevelNavigationComponent");

	SelectionStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("SelectionStaticMeshComponent");
	SelectionStaticMeshComponent->SetupAttachment(RootComponent);
}

void ABBCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	Actions.OnActionsChange.AddDynamic(this, &ABBCharacterBase::OnActionsChangedForwarder);
}

TArray<FRoomPosition> ABBCharacterBase::GetPath(FRoomPosition& Start, FRoomPosition& End)
{
	return LevelNavigationComponent->GetPath(Start, End);
}

TArray<FRoomPosition> ABBCharacterBase::GetPath(FRoomPosition& Target)
{
	return GetPath(CurrentRoom->Position, Target);
}

UBBLevelNavigationComponent* ABBCharacterBase::GetLevelNavigationComponent() const
{
	return LevelNavigationComponent;
}

// Called every frame
void ABBCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABBCharacterBase::InitCharacter(ABBLevel* Level, FRoomPosition InitialRoom)
{
	CurrentLevel = Level;
	CurrentRoom = CurrentLevel->GetRoomAt(InitialRoom);
	FVector Offset(0.f, 0.f, 100.f);
	if (Cast<ABBGuard>(this))
	{
		SetActorLocation(CurrentRoom->GetGuardPosition() + Offset);
	}
	else
	{
		SetActorLocation(CurrentRoom->GetFreePlayerPosition() + Offset);
	}
	SelectionStaticMeshComponent->SetVisibility(false);
}

void ABBCharacterBase::OnRoomEntered(ABBRoomBase* Room)
{
	CurrentRoom = Room;
	DOnRoomEntered.Broadcast(Room);
}

void ABBCharacterBase::OnRoomLeft(ABBRoomBase* Room)
{
	PreviousRoom = CurrentRoom;
	DOnRoomLeft.Broadcast(PreviousRoom);
}

void ABBCharacterBase::StartTurn()
{
	OnTurnStart.Broadcast();
	SelectionStaticMeshComponent->SetVisibility(true);
}

void ABBCharacterBase::FinishTurn()
{
	SelectionStaticMeshComponent->SetVisibility(false);
}

void ABBCharacterBase::OnActionsChangedForwarder()
{
	OnActionsChange.Broadcast();
}

void ABBCharacterBase::SetCharacterHidden(bool IsHidden)
{
	SetActorHiddenInGame(IsHidden);
}

