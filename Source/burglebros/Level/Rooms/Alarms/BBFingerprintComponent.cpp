// Fill out your copyright notice in the Description page of Project Settings.

#include "BBFingerprintComponent.h"
#include "BBRoomBase.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"

bool UBBFingerprintComponent::OnEntered(class ABBCharacterBase* Character)
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
		if (!IsHacker && !HackerPresent)
		{
			if (UBBUtilities::GetComputerRoomTokenAmount(GetOwner(), ERoomType::ComputerFingerprint) > 0
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

bool UBBFingerprintComponent::CanDisableRoom()
{
	return true;
}
