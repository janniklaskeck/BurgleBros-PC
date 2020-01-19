// Fill out your copyright notice in the Description page of Project Settings.

#include "BBMotionComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBRoomBase.h"
#include "BBMainGameMode.h"

void UBBMotionComponent::BeginPlay()
{
	Super::BeginPlay();
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (GameMode)
		GameMode->OnTurnEnd.AddDynamic(this, &UBBMotionComponent::OnTurnEnd);
}

bool UBBMotionComponent::OnEntered(class ABBCharacterBase* Character)
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
			EnteredCharacters.Add(Character);
	}
	return true;
}

void UBBMotionComponent::OnLeft(class ABBCharacterBase* Character)
{
	Super::OnLeft(Character);
	if (EnteredCharacters.Contains(Character))
	{
		EnteredCharacters.Remove(Character);
		if (UBBUtilities::GetComputerRoomTokenAmount(GetOwner(), ERoomType::ComputerMotion) > 0
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

bool UBBMotionComponent::CanDisableRoom()
{
	return true;
}

void UBBMotionComponent::OnTurnEnd()
{
	EnteredCharacters.Reset(4);
}
