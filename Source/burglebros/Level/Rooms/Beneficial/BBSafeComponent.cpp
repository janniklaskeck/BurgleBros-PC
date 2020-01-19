// Fill out your copyright notice in the Description page of Project Settings.

#include "BBSafeComponent.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBUtilities.h"
#include "BBLootLibrary.h"
#include "BBToolLibrary.h"
#include "BBMainGameMode.h"
#include "BBPlayerCharacterBase.h"
#include "BBGuard.h"

bool UBBSafeComponent::CanUse(class ABBCharacterBase* Character)
{
	return Super::CanUse(Character) && !bIsCracked;
}

void UBBSafeComponent::OnUsed(class ABBCharacterBase* Character)
{
	UBBUtilities::GetMainGameMode(GetWorld())->EnterRoomAction();
}

bool UBBSafeComponent::CanAddDie(class ABBCharacterBase* Character)
{
	return Character->Actions.GetCurrentActions() > 1;
}

bool UBBSafeComponent::CanThrowDice(class ABBCharacterBase* Character)
{
	bool NeedsKeycard = UBBLootLibrary::HasToolBeenDrawn(ELootType::Keycard);
	bool KeycardHolderPresent = false;
	if (NeedsKeycard)
	{
		for (ABBCharacterBase* CharacterInRoom : GetOwnerRoom()->CharactersInRoom)
		{
			ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(CharacterInRoom);
			if (PlayerCharacter && PlayerCharacter->Loot.Contains(ELootType::Keycard))
			{
				KeycardHolderPresent = true;
				break;
			}
		}
	}
	if (NeedsKeycard && !KeycardHolderPresent)
	{
		return false;
	}
	return AmountDice > 0 && Character->Actions.GetCurrentActions() > 0;
}

void UBBSafeComponent::AddDie(class ABBCharacterBase* Character)
{
	++AmountDice;
	Character->Actions.ConsumeActions(2);
}

void UBBSafeComponent::RollDice(class ABBCharacterBase* Character)
{
	Character->Actions.ConsumeAction();
	int32 AmountOfDice = AmountDice;
	if (Cast<ABBPlayerCharacterBase>(Character)->PlayerType == EPlayerClass::Peterman)
	{
		++AmountOfDice;
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->RollDiceAmount = AmountOfDice;
	GameMode->DOnLeaveGameAction.AddDynamic(this, &UBBSafeComponent::ApplyRollResults);
	GameMode->EnterRollDice();
}

void UBBSafeComponent::ApplyRollResults()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (GameMode->GameActionEnterEvent != EBBMainGameEvents::RollDiceEnter)
		return;
	TArray<ABBRoomBase*> DiscoveredRooms = GetNecessaryDiceResults(false);

	for (ABBRoomBase* Room : DiscoveredRooms)
	{
		if (Room->bIsDiscovered && GameMode->RollDiceResults.Contains(Room->SafeCombinationNumber))
		{
			Room->bSafeCombinationNumberCracked = true;
		}
	}
	bool HasFullCombination = true;
	for (ABBRoomBase* Room : DiscoveredRooms)
	{
		if (!Room->bSafeCombinationNumberCracked)
		{
			HasFullCombination = false;
			break;
		}
	}
	if (HasFullCombination)
	{
		if (GetOwnerRoom()->CharactersInRoom.Contains(GameMode->GetCurrentPlayerCharacter()))
		{
			UBBLootLibrary::DrawRandomLoot(GameMode->GetCurrentPlayerCharacter());
			GameMode->EnterDrawCard();
		}
		else
		{
			UBBLootLibrary::DrawRandomLoot(GetOwnerRoom());
			UBBToolLibrary::DrawRandomToolFromDeck(GetOwnerRoom());
		}
		bIsCracked = true;
		for (int32 Index = GetOwnerRoom()->Position.Floor; Index >= 0; Index--)
		{
			ABBGuard* GuardOfFloor = GameMode->GetGuard(Index);
			if (GuardOfFloor)
				GuardOfFloor->Actions.SetMaxActions(GuardOfFloor->Actions.GetMaxActions() + 1);
		}
	}
	else
	{
		GameMode->EnterRoomAction();
	}
	GameMode->UpdateSelectedRoom();
	GameMode->DOnLeaveGameAction.RemoveDynamic(this, &UBBSafeComponent::ApplyRollResults);
}

TArray<ABBRoomBase*> UBBSafeComponent::GetNecessaryDiceResults(bool CheckForDiscovered)
{
	TArray<ABBRoomBase*> DiscoveredRooms;
	ABBLevel* Level = GetOwnerRoom()->CurrentLevel;
	FRoomPosition Position = GetOwnerRoom()->Position;
	for (int32 Index = 0; Index < Level->FloorWidth; Index++)
	{
		ABBRoomBase* XRoom = Level->GetRoomAt(Index, Position.PosY, Position.Floor);
		if (XRoom->RoomType != ERoomType::Safe && (!CheckForDiscovered || XRoom->bIsDiscovered))
			DiscoveredRooms.Add(XRoom);
		ABBRoomBase* YRoom = Level->GetRoomAt(Position.PosX, Index, Position.Floor);
		if (YRoom->RoomType != ERoomType::Safe && (!CheckForDiscovered || YRoom->bIsDiscovered))
			DiscoveredRooms.Add(YRoom);
	}
	return DiscoveredRooms;
}

TArray<int32> UBBSafeComponent::GetSafeCombination()
{
	TArray<ABBRoomBase*> NecessaryDiceResults = GetNecessaryDiceResults(true);
	TArray<int32> SafeCombination;
	for (ABBRoomBase* Room : NecessaryDiceResults)
	{
		SafeCombination.AddUnique(Room->SafeCombinationNumber);
	}
	return SafeCombination;
}

bool UBBSafeComponent::IsNumberAlreadyCracked(int32 Number)
{
	TArray<ABBRoomBase*> DiscoveredRooms = GetNecessaryDiceResults(true);

	for (ABBRoomBase* Room : DiscoveredRooms)
	{
		if (Room->SafeCombinationNumber == Number && Room->bSafeCombinationNumberCracked)
		{
			return true;
		}
	}
	return false;
}
