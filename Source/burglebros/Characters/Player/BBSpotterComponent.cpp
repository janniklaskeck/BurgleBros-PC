// Fill out your copyright notice in the Description page of Project Settings.

#include "BBSpotterComponent.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "BBEventLibrary.h"
#include "BBGuard.h"
#include "BBPlayerCharacterBase.h"

bool UBBSpotterComponent::CanUseAbility()
{
	if (GetOwnerCharacter()->Actions.GetCurrentActions() == 0)
	{
		return false;
	}
	return !bUsedAbility;
}

bool UBBSpotterComponent::IsActiveAbility()
{
	return true;
}

void UBBSpotterComponent::UseAbility()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GetOwnerCharacter()->Actions.ConsumeAction();
	GameMode->EnterCharacterAction();
}

void UBBSpotterComponent::MoveTopEventToBottom()
{
	bUsedAbility = true;
	UBBEventLibrary::MoveFirstEventToBottom();
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->LeaveGameAction();
}

void UBBSpotterComponent::MoveTopPatrolToBottom()
{
	bUsedAbility = true;
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->GetCurrentGuardPawn()->MoveTopPatrolTargetToBottom();
	GameMode->LeaveGameAction();
}
