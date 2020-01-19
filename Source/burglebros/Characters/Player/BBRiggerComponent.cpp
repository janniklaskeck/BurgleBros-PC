// Fill out your copyright notice in the Description page of Project Settings.

#include "BBRiggerComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBToolLibrary.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"

void UBBRiggerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (PlayerClass == EPlayerClass::Rigger)
	{
		UBBToolLibrary::GetToolFromDeck(EToolType::Dynamite);
		GetOwnerCharacter()->Tools.Add(EToolType::Dynamite);
	}
}

bool UBBRiggerComponent::CanUseAbility()
{
	return IsActiveAbility() && GetOwnerCharacter()->GetStealthTokens() > 0;
}

bool UBBRiggerComponent::IsActiveAbility()
{
	return PlayerClass == EPlayerClass::Rigger_Alt;
}

void UBBRiggerComponent::UseAbility()
{
	GetOwnerCharacter()->ConsumeStealthToken();
	UBBUtilities::GetMainGameMode(this)->EnterDrawCard();
}
