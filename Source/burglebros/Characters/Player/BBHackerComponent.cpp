// Fill out your copyright notice in the Description page of Project Settings.

#include "BBHackerComponent.h"
#include "BBPlayerCharacterBase.h"

bool UBBHackerComponent::CanUseAbility()
{
	return IsActiveAbility() && !bHasToken && GetOwnerCharacter()->Actions.GetCurrentActions() > 0;
}

bool UBBHackerComponent::IsActiveAbility()
{
	return PlayerClass == EPlayerClass::Hacker_Alt;
}

void UBBHackerComponent::UseAbility()
{
	bHasToken = true;
	GetOwnerCharacter()->Actions.ConsumeAction();
}

bool UBBHackerComponent::HasToken()
{
	return bHasToken;
}

void UBBHackerComponent::UseToken()
{
	bHasToken = false;
}
