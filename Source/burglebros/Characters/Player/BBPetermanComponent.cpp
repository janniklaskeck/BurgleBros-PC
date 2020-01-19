// Fill out your copyright notice in the Description page of Project Settings.

#include "BBPetermanComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBPlayerCameraManager.h"
#include "BBLevel.h"
#include "BBRoomBase.h"
#include "BBUtilities.h"
#include "BBSafeComponent.h"
#include "BBMainGameMode.h"

bool UBBPetermanComponent::CanUseAbility()
{
	if (GetOwnerCharacter()->Actions.GetCurrentActions() == 0)
	{
		return false;
	}
	return GetBelowSafeRoom() || GetAboveSafeRoom();
}

bool UBBPetermanComponent::IsActiveAbility()
{
	return GetOwnerCharacter()->PlayerType == EPlayerClass::Peterman_Alt;
}

void UBBPetermanComponent::UseAbility()
{
	ABBRoomBase* BelowRoom = GetBelowSafeRoom();
	if (BelowRoom)
	{
		if (BelowRoom->GetRoomFunction()->CanUse(GetOwnerCharacter()))
		{
			UBBUtilities::GetPlayerCameraManager(GetOwner(), 0)->SetCustomViewtarget(BelowRoom->GetActorLocation(), BelowRoom->Position.Floor);
			BelowRoom->GetRoomFunction()->OnUse(GetOwnerCharacter());
			UBBUtilities::GetMainGameMode(this)->RoomActionRoom = BelowRoom;
		}
		return;
	}
	ABBRoomBase* AboveRoom = GetAboveSafeRoom();
	if (AboveRoom)
	{
		if (AboveRoom->GetRoomFunction()->CanUse(GetOwnerCharacter()))
		{
			UBBUtilities::GetPlayerCameraManager(GetOwner(), 0)->SetCustomViewtarget(AboveRoom->GetActorLocation(), AboveRoom->Position.Floor);
			AboveRoom->GetRoomFunction()->OnUse(GetOwnerCharacter());
			UBBUtilities::GetMainGameMode(this)->RoomActionRoom = AboveRoom;
		}
	}
}

class ABBRoomBase* UBBPetermanComponent::GetBelowSafeRoom()
{
	FRoomPosition CurrentPosition = GetOwnerCharacter()->CurrentRoom->Position;
	FRoomPosition BelowRoomPosition = CurrentPosition.GetBelowRoomPosition();
	ABBRoomBase* BelowRoom = GetOwnerCharacter()->CurrentLevel->GetRoomAt(BelowRoomPosition);
	if (BelowRoom)
	{
		if (BelowRoom->RoomType == ERoomType::Safe && !BelowRoom->GetRoomFunctionAs<UBBSafeComponent>()->bIsCracked)
		{
			return BelowRoom;
		}
	}
	return nullptr;
}

class ABBRoomBase* UBBPetermanComponent::GetAboveSafeRoom()
{
	FRoomPosition CurrentPosition = GetOwnerCharacter()->CurrentRoom->Position;
	FRoomPosition AboveRoomPosition = CurrentPosition.GetAboveRoomPosition();
	ABBRoomBase* AboveRoom = GetOwnerCharacter()->CurrentLevel->GetRoomAt(AboveRoomPosition);
	if (AboveRoom)
	{
		if (AboveRoom->RoomType == ERoomType::Safe && !AboveRoom->GetRoomFunctionAs<UBBSafeComponent>()->bIsCracked)
		{
			return AboveRoom;
		}
	}
	return nullptr;
}

