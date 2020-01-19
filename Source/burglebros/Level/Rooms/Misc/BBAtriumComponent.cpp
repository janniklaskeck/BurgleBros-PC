// Fill out your copyright notice in the Description page of Project Settings.

#include "BBAtriumComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBMainGameMode.h"
#include "BBUtilities.h"

void UBBAtriumComponent::BeginPlay()
{
	Super::BeginPlay();
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (GameMode)
		GameMode->OnInitialization.AddDynamic(this, &UBBAtriumComponent::BindToNeighbourRooms);
}

bool UBBAtriumComponent::OnEntered(class ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	if (!IsGuard(Character))
	{
		ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
		bool AlreadySeen = false;
		ABBRoomBase* RoomAbove = GetRoomAbove();
		if (RoomAbove && RoomAbove->ContainsGuard())
		{
			PlayerCharacter->ConsumeStealthToken();
			AlreadySeen = true;
		}
		ABBRoomBase* RoomBelow = GetRoomBelow();
		if (!AlreadySeen && RoomBelow && RoomBelow->ContainsGuard())
		{
			PlayerCharacter->ConsumeStealthToken();
		}
	}
	return true;
}

bool UBBAtriumComponent::CanUse(class ABBCharacterBase* Character)
{
	return !IsGuard(Character) && Character->Actions.GetCurrentActions() > 0 && GetOwnerRoom()->CharactersInRoom.Contains(Character);
}

void UBBAtriumComponent::OnUsed(class ABBCharacterBase* Character)
{
	if (!IsGuard(Character))
	{
		UBBUtilities::GetMainGameMode(GetWorld())->EnterRoomAction();
	}
}

bool UBBAtriumComponent::CanPeekAbove(class ABBCharacterBase* Character)
{
	int32 AmountFloors = GetOwnerRoom()->CurrentLevel->AmountFloors;
	FRoomPosition CurrentPosition = GetOwnerRoom()->Position;
	if (AmountFloors > 1 && CurrentPosition.Floor < AmountFloors - 1 && Character->Actions.GetCurrentActions() > 0)
	{
		FRoomPosition RoomAbove(CurrentPosition.PosX, CurrentPosition.PosY, CurrentPosition.Floor + 1);
		return !GetOwnerRoom()->CurrentLevel->GetRoomAt(RoomAbove)->bIsDiscovered;
	}
	return false;
}

void UBBAtriumComponent::PeekAbove(class ABBCharacterBase* Character)
{
	FRoomPosition CurrentPosition = GetOwnerRoom()->Position;
	FRoomPosition RoomAbove(CurrentPosition.PosX, CurrentPosition.PosY, CurrentPosition.Floor + 1);
	GetOwnerRoom()->CurrentLevel->GetRoomAt(RoomAbove)->SetDiscovered();
	Character->Actions.ConsumeAction();
}

bool UBBAtriumComponent::CanPeekBelow(class ABBCharacterBase* Character)
{
	int32 AmountFloors = GetOwnerRoom()->CurrentLevel->AmountFloors;
	FRoomPosition CurrentPosition = GetOwnerRoom()->Position;
	if (AmountFloors > 1 && CurrentPosition.Floor > 0 && Character->Actions.GetCurrentActions() > 0)
	{
		FRoomPosition RoomBelow(CurrentPosition.PosX, CurrentPosition.PosY, CurrentPosition.Floor - 1);
		return !GetOwnerRoom()->CurrentLevel->GetRoomAt(RoomBelow)->bIsDiscovered;
	}
	return false;
}

void UBBAtriumComponent::PeekBelow(class ABBCharacterBase* Character)
{
	FRoomPosition CurrentPosition = GetOwnerRoom()->Position;
	FRoomPosition RoomBelow(CurrentPosition.PosX, CurrentPosition.PosY, CurrentPosition.Floor - 1);
	GetOwnerRoom()->CurrentLevel->GetRoomAt(RoomBelow)->SetDiscovered();
	Character->Actions.ConsumeAction();
}

class ABBRoomBase* UBBAtriumComponent::GetRoomAbove()
{
	FRoomPosition CurrentPosition = GetOwnerRoom()->Position;
	int32 CurrentFloor = CurrentPosition.Floor;
	int32 AmountFloors = GetOwnerRoom()->CurrentLevel->AmountFloors;
	if (CurrentFloor + 1 < AmountFloors)
	{
		FRoomPosition AbovePosition(CurrentPosition.PosX, CurrentPosition.PosY, CurrentFloor + 1);
		return GetOwnerRoom()->CurrentLevel->GetRoomAt(AbovePosition);
	}
	return nullptr;
}

class ABBRoomBase* UBBAtriumComponent::GetRoomBelow()
{
	FRoomPosition CurrentPosition = GetOwnerRoom()->Position;
	int32 CurrentFloor = CurrentPosition.Floor;
	if (CurrentFloor + 1 > 1)
	{
		FRoomPosition BelowPosition(CurrentPosition.PosX, CurrentPosition.PosY, CurrentFloor - 1);
		return GetOwnerRoom()->CurrentLevel->GetRoomAt(BelowPosition);
	}
	return nullptr;
}

void UBBAtriumComponent::BindToNeighbourRooms()
{
	ABBRoomBase* RoomAbove = GetRoomAbove();
	if (RoomAbove)
	{
		RoomAbove->OnRoomCharacterEnter.AddDynamic(this, &UBBAtriumComponent::CheckForGuard);
	}
	ABBRoomBase* RoomBelow = GetRoomBelow();
	if (RoomBelow)
	{
		RoomBelow->OnRoomCharacterEnter.AddDynamic(this, &UBBAtriumComponent::CheckForGuard);
	}
}

void UBBAtriumComponent::CheckForGuard(class ABBCharacterBase* CharacterEntered)
{
	if (IsGuard(CharacterEntered))
	{
		for (ABBCharacterBase* Character : GetOwnerRoom()->CharactersInRoom)
		{
			if (!IsGuard(Character))
			{
				Cast<ABBPlayerCharacterBase>(Character)->ConsumeStealthToken();
			}
		}
	}
}
