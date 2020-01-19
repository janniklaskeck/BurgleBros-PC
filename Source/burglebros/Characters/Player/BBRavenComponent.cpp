// Fill out your copyright notice in the Description page of Project Settings.

#include "BBRavenComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBRoomBase.h"
#include "BBMainGameMode.h"
#include "BBUtilities.h"
#include "BBLevel.h"

bool UBBRavenComponent::CanUseAbility()
{
	if (GetOwnerCharacter()->Actions.GetCurrentActions() == 0)
	{
		return false;
	}
	if (PlayerClass == EPlayerClass::Raven)
	{
		return !bUsedAbility;
	}
	else
	{
		return !bUsedAbility && !GetOwnerCharacter()->CurrentRoom->bContainsRavenAlt && GetOwnerCharacter()->CurrentRoom != RoomWithRaven;
	}
}

bool UBBRavenComponent::IsActiveAbility()
{
	return true;
}

void UBBRavenComponent::UseAbility()
{
	if (PlayerClass == EPlayerClass::Raven)
	{
		ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
		GameMode->SetValidRoomsToChoose(GetValidRavenRooms());
		GameMode->OnRoomChosen.AddDynamic(this, &UBBRavenComponent::OnRavenRoomChosen);
		GameMode->EnterChooseRoom();
	}
	else
	{
		if (RoomWithRaven)
		{
			RoomWithRaven->SetRavenInRoom(false, true);
		}
		RoomWithRaven = GetOwnerCharacter()->CurrentRoom;
		RoomWithRaven->SetRavenInRoom(true, true);
		bUsedAbility = true;
	}
}

TArray<class ABBRoomBase*> UBBRavenComponent::GetValidRavenRooms()
{
	ABBPlayerCharacterBase* OwnerCharacter = GetOwnerCharacter();
	TArray<ABBRoomBase*> RoomsOfFloor = OwnerCharacter->CurrentLevel->GetRoomsOfFloor(OwnerCharacter);
	TArray<ABBRoomBase*> ValidRooms = RoomsOfFloor.FilterByPredicate([OwnerCharacter](ABBRoomBase* Room)
	{
		FRoomPosition CurrentPosition = OwnerCharacter->CurrentRoom->Position;
		bool XPosInRange = FMath::Abs(Room->Position.PosX - CurrentPosition.PosX) <= 2;
		bool YPosInRange = FMath::Abs(Room->Position.PosY - CurrentPosition.PosY) <= 2;
		if (XPosInRange && YPosInRange)
		{
			return OwnerCharacter->GetPath(Room->Position).Num() <= 2;
		}
		return false;
	});
	return ValidRooms;
}

void UBBRavenComponent::OnRavenRoomChosen(class ABBRoomBase* ChosenRoom)
{
	if (RoomWithRaven)
	{
		RoomWithRaven->SetRavenInRoom(false, false);
	}
	if (ChosenRoom)
	{
		RoomWithRaven = ChosenRoom;
		RoomWithRaven->bContainsRaven = true;
		bUsedAbility = true;
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->OnRoomChosen.RemoveDynamic(this, &UBBRavenComponent::OnRavenRoomChosen);
}
