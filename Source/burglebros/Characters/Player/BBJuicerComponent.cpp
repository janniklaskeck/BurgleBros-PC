// Fill out your copyright notice in the Description page of Project Settings.

#include "BBJuicerComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBRoomBase.h"
#include "BBMainGameMode.h"
#include "BBUtilities.h"
#include "BBLevel.h"

bool UBBJuicerComponent::CanUseAbility()
{
	if (PlayerClass == EPlayerClass::Juicer_Alt)
	{
		return !bUsedAbility && (bHasAlarmToken || GetOwnerCharacter()->CurrentRoom->bHasAlarm);
	}
	return !bUsedAbility;
}

bool UBBJuicerComponent::IsActiveAbility()
{
	return true;
}

void UBBJuicerComponent::UseAbility()
{
	if (PlayerClass == EPlayerClass::Juicer)
	{
		ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
		GameMode->OnRoomChosen.AddDynamic(this, &UBBJuicerComponent::OnRoomChosen);
		GameMode->SetValidRoomsToChoose(GetValidRooms());
		GameMode->EnterChooseRoom();
	}
	else
	{
		if (bHasAlarmToken)
		{
			bUsedAbility = true;
			bHasAlarmToken = false;
			GetOwnerCharacter()->CurrentRoom->SetAlarm(true);
		}
		else if (GetOwnerCharacter()->CurrentRoom->bHasAlarm)
		{
			bUsedAbility = true;
			bHasAlarmToken = true;
			GetOwnerCharacter()->CurrentRoom->SetAlarm(false);
		}
	}
}

TArray<class ABBRoomBase*> UBBJuicerComponent::GetValidRooms()
{
	ABBPlayerCharacterBase* OwnerCharacter = GetOwnerCharacter();
	FRoomPosition CurrentPosition = OwnerCharacter->CurrentRoom->Position;
	TArray<ABBRoomBase*> RoomsOfFloor = OwnerCharacter->CurrentLevel->GetRoomsOfFloor(OwnerCharacter);
	TArray<ABBRoomBase*> ValidRooms = RoomsOfFloor.FilterByPredicate([&CurrentPosition, OwnerCharacter](ABBRoomBase* Room)
	{
		return Room->Position.IsNextToRoom(CurrentPosition) && OwnerCharacter->GetPath(Room->Position).Num() == 1;
	});
	return ValidRooms;
}

void UBBJuicerComponent::OnRoomChosen(class ABBRoomBase* ChosenRoom)
{
	if (ChosenRoom)
	{
		bUsedAbility = true;
		ChosenRoom->SetAlarm(true);
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->OnRoomChosen.RemoveDynamic(this, &UBBJuicerComponent::OnRoomChosen);
}
