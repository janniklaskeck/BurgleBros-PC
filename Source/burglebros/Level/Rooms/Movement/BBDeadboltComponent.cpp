// Fill out your copyright notice in the Description page of Project Settings.

#include "BBDeadboltComponent.h"
#include "BBRoomBase.h"
#include "BBCharacterBase.h"

int32 UBBDeadboltComponent::GetEnterCost(class ABBCharacterBase* Character)
{
	if (GetOwnerRoom()->CharactersInRoom.Num() > 0)
	{
		return Super::GetEnterCost(Character);
	}
	return 3;
}

bool UBBDeadboltComponent::CanEnter(ABBCharacterBase* Character)
{
	if (IsGuard(Character))
	{
		return true;
	}
	return Super::CanEnter(Character);
}

bool UBBDeadboltComponent::CanDisableRoom()
{
	return true;
}
