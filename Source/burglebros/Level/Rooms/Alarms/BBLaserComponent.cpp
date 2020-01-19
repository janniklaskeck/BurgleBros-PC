// Fill out your copyright notice in the Description page of Project Settings.

#include "BBLaserComponent.h"
#include "BBUtilities.h"
#include "BBPlayerCharacterBase.h"
#include "BBMainGameMode.h"
#include "BBRoomBase.h"

bool UBBLaserComponent::OnEntered(class ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	if (!IsGuard(Character))
	{
		bool IsHacker = Cast<ABBPlayerCharacterBase>(Character)->PlayerType == EPlayerClass::Hacker;
		bool HackerPresent = GetOwnerRoom()->CharactersInRoom.ContainsByPredicate([](ABBCharacterBase* CharacterInRoom)
		{
			ABBPlayerCharacterBase* Player = Cast<ABBPlayerCharacterBase>(CharacterInRoom);
			return Player && Player->PlayerType == EPlayerClass::Hacker;
		});
		if (IsHacker || HackerPresent)
			return true;
		if (GetOwnerRoom()->bIsDiscovered)
		{
			UBBUtilities::GetMainGameMode(Character)->EnterRoomAction();
			return false;
		}
		else
		{
			if (UBBUtilities::GetComputerRoomTokenAmount(GetOwner(), ERoomType::ComputerLaser) > 0
				|| UBBUtilities::HackerTokenPresent(GetOwner()))
			{
				ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
				GameMode->PreventAlarmRoom = GetOwnerRoom();
				GameMode->EnterPreventAlarm();
			}
			else
			{
				GetOwnerRoom()->SetAlarm(true);
			}
		}
	}
	return true;
}

bool UBBLaserComponent::CanDisableRoom()
{
	return true;
}

void UBBLaserComponent::EnterWithOneAction(class ABBCharacterBase* Character)
{
	ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
	if (!PlayerCharacter->Loot.Contains(ELootType::Mirror))
	{
		if (UBBUtilities::GetComputerRoomTokenAmount(GetOwner(), ERoomType::ComputerLaser) > 0)
		{
			ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
			GameMode->PreventAlarmRoom = GetOwnerRoom();
			GameMode->EnterPreventAlarm();
		}
		else
		{
			GetOwnerRoom()->SetAlarm(true);
			UBBUtilities::GetMainGameMode(Character)->LeaveGameAction();
		}
	}
	else
	{
		UBBUtilities::GetMainGameMode(Character)->LeaveGameAction();
	}
}

void UBBLaserComponent::EnterWithTwoAction(class ABBCharacterBase* Character)
{
	Character->Actions.ConsumeAction();
	UBBUtilities::GetMainGameMode(Character)->LeaveGameAction();
}

bool UBBLaserComponent::CanEnterWithTwoActions(class ABBCharacterBase* Character)
{
	ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
	bool EnoughActions = Character->Actions.GetCurrentActions() > 0;
	return EnoughActions && !PlayerCharacter->Loot.Contains(ELootType::Mirror);
}
