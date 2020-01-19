// Fill out your copyright notice in the Description page of Project Settings.

#include "BBComputerComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBRoomBase.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"

bool UBBComputerComponent::CanUse(class ABBCharacterBase* Character)
{
	if (!GetOwnerRoom()->CharactersInRoom.Contains(Character))
		return false;
	return CanAddToken(Character) && Super::CanUse(Character);
}

void UBBComputerComponent::OnUsed(class ABBCharacterBase* Character)
{
	UBBUtilities::GetMainGameMode(GetWorld())->EnterRoomAction();
}

void UBBComputerComponent::UseToken()
{
	AmountTokens = FMath::Clamp(--AmountTokens, 0, MaxTokens);
}

void UBBComputerComponent::AddToken(class ABBCharacterBase* Character)
{
	AmountTokens = FMath::Clamp(++AmountTokens, 0, MaxTokens);
	Character->Actions.ConsumeAction();
}

void UBBComputerComponent::AddTokens(int32 Amount)
{
	AmountTokens = FMath::Clamp(AmountTokens + Amount, 0, MaxTokens);
}

bool UBBComputerComponent::CanAddToken(class ABBCharacterBase* Character)
{
	return AmountTokens < MaxTokens && Character->Actions.GetCurrentActions() > 0;
}
