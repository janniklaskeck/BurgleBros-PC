// Fill out your copyright notice in the Description page of Project Settings.

#include "BBKeypadComponent.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "BBPlayerCharacterBase.h"
#include "BBLevelNavigationComponent.h"
#include "BBRoomBase.h"

void UBBKeypadComponent::BeginPlay()
{
	Super::BeginPlay();
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (GameMode)
		GameMode->OnTurnEnd.AddDynamic(this, &UBBKeypadComponent::ResetTries);
}

bool UBBKeypadComponent::CanEnter(ABBCharacterBase* Character)
{
	return Super::CanEnter(Character);
}

bool UBBKeypadComponent::OnEntered(ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	if (!IsGuard(Character) && !bIsUnlocked)
	{
		++AmountTries;
		int32 AmountDice = AmountTries;
		if (Cast<ABBPlayerCharacterBase>(Character)->PlayerType == EPlayerClass::Peterman)
		{
			++AmountDice;
		}
		ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
		GameMode->RollDiceAmount = AmountDice;
		GameMode->DOnLeaveGameAction.AddDynamic(this, &UBBKeypadComponent::CheckResults);
		GameMode->EnterRollDice();
		return false;
	}
	return true;
}

bool UBBKeypadComponent::CanDisableRoom()
{
	return true;
}

void UBBKeypadComponent::ResetTries()
{
	AmountTries = 0;
}

void UBBKeypadComponent::CheckResults()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	if (GameMode->RollDiceResults.Contains(6))
	{
		bIsUnlocked = true;
		AmountTries = 0;
		OnEnter(GameMode->GetCurrentPlayerCharacter(), false, true);
	}
	else
	{
		ABBPlayerCharacterBase* CurrentCharacter = GameMode->GetCurrentPlayerCharacter();
		if (CurrentCharacter->PreviousRoom)
		{
			if (GetOwnerRoom()->Position.Floor == CurrentCharacter->PreviousRoom->Position.Floor)
			{
				CurrentCharacter->GetLevelNavigationComponent()->MoveToRoom(CurrentCharacter->PreviousRoom, true, true);
			}
			else
			{
				ABBRoomBase* PreviousRoom = CurrentCharacter->PreviousRoom;
				CurrentCharacter->CurrentRoom->LeaveRoom(CurrentCharacter);
				PreviousRoom->EnterRoom(CurrentCharacter, false, false);
				PreviousRoom->TeleportToRoom(CurrentCharacter, true);
			}
		}
	}
	GameMode->DOnLeaveGameAction.RemoveDynamic(this, &UBBKeypadComponent::CheckResults);
}
