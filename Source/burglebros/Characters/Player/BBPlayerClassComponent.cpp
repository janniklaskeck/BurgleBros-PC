// Fill out your copyright notice in the Description page of Project Settings.

#include "BBPlayerClassComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"

UBBPlayerClassComponent::UBBPlayerClassComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBBPlayerClassComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerClass = GetOwnerCharacter()->PlayerType;
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->DOnEnterPlayerTurn.AddDynamic(this, &UBBPlayerClassComponent::ResetUsed);
}

class ABBPlayerCharacterBase* UBBPlayerClassComponent::GetOwnerCharacter()
{
	return Cast<ABBPlayerCharacterBase>(GetOwner());
}

bool UBBPlayerClassComponent::CanUseAbility()
{
	return false;
}

bool UBBPlayerClassComponent::IsActiveAbility()
{
	return false;
}

void UBBPlayerClassComponent::UseAbility()
{

}

void UBBPlayerClassComponent::ResetUsed()
{
	bUsedAbility = false;
}
