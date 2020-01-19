// Fill out your copyright notice in the Description page of Project Settings.

#include "BBFoyerComponent.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"

void UBBFoyerComponent::BeginPlay()
{
	Super::BeginPlay();
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (GameMode)
		GameMode->OnInitialization.AddDynamic(this, &UBBFoyerComponent::BindToNeighbourRooms);
}

bool UBBFoyerComponent::OnEntered(class ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	if (!IsGuard(Character))
	{
		TArray<ABBRoomBase*> RoomsAround = UBBUtilities::GetRoomsAround(GetOwnerRoom());
		for (ABBRoomBase* Room : RoomsAround)
		{
			if (Room->ContainsGuard())
			{
				ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
				if (!PlayerCharacter->bIsInvisible)
					PlayerCharacter->ConsumeStealthToken();
			}
		}
	}
	return true;
}

void UBBFoyerComponent::BindToNeighbourRooms()
{
	TArray<ABBRoomBase*> RoomsAround = UBBUtilities::GetRoomsAround(GetOwnerRoom());
	for (ABBRoomBase* Room : RoomsAround)
	{
		Room->OnRoomCharacterEnter.AddDynamic(this, &UBBFoyerComponent::CheckForGuard);
	}
}

void UBBFoyerComponent::CheckForGuard(class ABBCharacterBase* CharacterEntered)
{
	if (IsGuard(CharacterEntered))
	{
		for (ABBCharacterBase* Character : GetOwnerRoom()->CharactersInRoom)
		{
			if (!IsGuard(Character))
			{
				ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
				if (!PlayerCharacter->bIsInvisible)
					PlayerCharacter->ConsumeStealthToken();
			}
		}
	}
}
