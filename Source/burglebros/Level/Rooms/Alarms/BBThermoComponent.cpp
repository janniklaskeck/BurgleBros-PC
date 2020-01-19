// Fill out your copyright notice in the Description page of Project Settings.

#include "BBThermoComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBRoomBase.h"
#include "BBMainGameMode.h"
#include "BBLootInfo.h"

void UBBThermoComponent::BeginPlay()
{
	Super::BeginPlay();
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (GameMode)
		GameMode->OnTurnEnd.AddDynamic(this, &UBBThermoComponent::OnTurnEnd);
}

bool UBBThermoComponent::OnEntered(class ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	if (!IsGuard(Character))
	{
		ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
		if (PlayerCharacter->Loot.Contains(ELootType::Isotope))
		{
			GetOwnerRoom()->SetAlarm(true);
		}
		else
		{
			EnteredCharacters.Add(Character);
		}
	}
	return true;
}

void UBBThermoComponent::OnLeft(class ABBCharacterBase* Character)
{
	Super::OnLeft(Character);
	if (!IsGuard(Character))
	{
		EnteredCharacters.Remove(Character);
	}
}

bool UBBThermoComponent::CanDisableRoom()
{
	return true;
}

void UBBThermoComponent::OnTurnEnd()
{
	if (EnteredCharacters.Num() > 0)
	{
		GetOwnerRoom()->SetAlarm(true);
	}
	EnteredCharacters.Reset(4);
}
