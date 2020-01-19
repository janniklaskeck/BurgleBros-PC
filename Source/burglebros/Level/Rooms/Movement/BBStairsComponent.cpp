// Fill out your copyright notice in the Description page of Project Settings.

#include "BBStairsComponent.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBPlayerCharacterBase.h"
#include "BBEventLibrary.h"

bool UBBStairsComponent::CanUse(ABBCharacterBase* Character)
{
	return false;
}

void UBBStairsComponent::OnUsed(ABBCharacterBase* Character)
{
}

bool UBBStairsComponent::CanDisableRoom()
{
	return false;
}
