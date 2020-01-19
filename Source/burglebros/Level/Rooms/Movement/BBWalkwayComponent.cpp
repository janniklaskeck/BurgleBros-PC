// Fill out your copyright notice in the Description page of Project Settings.

#include "BBWalkwayComponent.h"
#include "BBRoomBase.h"
#include "BBCharacterBase.h"
#include "BBLevel.h"

bool UBBWalkwayComponent::OnEntered(ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	if (!GetOwnerRoom()->bIsDiscovered && !IsGuard(Character))
	{
		MoveCharacterDown(Character);
		return false;
	}
	return true;
}

bool UBBWalkwayComponent::CanUse(ABBCharacterBase* Character)
{
	return false;
}

void UBBWalkwayComponent::OnUsed(ABBCharacterBase* Character)
{
}

void UBBWalkwayComponent::MoveCharacterDown(class ABBCharacterBase* Character)
{
	FRoomPosition CurrentRoomPosition = GetOwnerRoom()->Position;
	int32 CurrentFloor = GetOwnerRoom()->Position.Floor;
	if (CurrentFloor > 0)
	{
		FRoomPosition NewPosition = CurrentRoomPosition.GetBelowRoomPosition();
		ABBRoomBase* RoomBelow = Character->CurrentLevel->GetRoomAt(NewPosition);
		Character->CurrentRoom->LeaveRoom(Character);
		RoomBelow->EnterRoom(Character, true, true);
		RoomBelow->TeleportToRoom(Character);
	}
}
