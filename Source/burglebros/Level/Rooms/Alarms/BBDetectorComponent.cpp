// Fill out your copyright notice in the Description page of Project Settings.

#include "BBDetectorComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBRoomBase.h"

bool UBBDetectorComponent::OnEntered(class ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	if (!IsGuard(Character))
	{
		ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
		if (PlayerCharacter->Tools.Num() > 0 || PlayerCharacter->Loot.Num() > 0)
		{
			GetOwnerRoom()->SetAlarm(true);
		}
	}
	return true;
}

bool UBBDetectorComponent::CanDisableRoom()
{
	return true;
}

