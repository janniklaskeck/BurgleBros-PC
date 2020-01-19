// Fill out your copyright notice in the Description page of Project Settings.

#include "BBLootLibrary.h"
#include "BBPlayerCharacterBase.h"
#include "BBRoomBase.h"

TArray<ELootType> UBBLootLibrary::LootDeck;

void UBBLootLibrary::InitLootDeck()
{
	LootDeck.Reset(13);
	for (int32 Index = 0; Index < 12; Index++)
	{
		LootDeck.Add((ELootType)Index);
	}
	LootDeck.Add(ELootType::GoldBar); // Add second gold bar
}

void UBBLootLibrary::DrawRandomLoot(class ABBPlayerCharacterBase* Character)
{
	if (LootDeck.Num() == 0)
	{
		InitLootDeck();
	}
	int32 RandomLootIndex = FMath::RandRange(0, LootDeck.Num() - 1);
	ELootType RandomLoot = LootDeck[RandomLootIndex];
	LootDeck.RemoveAt(RandomLootIndex);
	if (RandomLoot == ELootType::CursedGoblet)
	{
		if (Character->GetStealthTokens() > 0)
			Character->ConsumeStealthToken();
	}
	else if (RandomLoot == ELootType::GoldBar)
	{
		if (LootDeck.Contains(RandomLoot))
		{
			Character->CurrentRoom->AddLootToRoom(RandomLoot);
			LootDeck.Remove(RandomLoot);
		}
	}
	AttachLootToPlayer(RandomLoot, Character);
}

void UBBLootLibrary::DrawRandomLoot(class ABBRoomBase* Room)
{
	if (LootDeck.Num() == 0)
	{
		InitLootDeck();
	}
	int32 RandomLootIndex = FMath::RandRange(0, LootDeck.Num() - 1);
	ELootType RandomLoot = LootDeck[RandomLootIndex];
	LootDeck.RemoveAt(RandomLootIndex);
	Room->AddLootToRoom(RandomLoot);
	if (RandomLoot == ELootType::GoldBar)
	{
		Room->AddLootToRoom(RandomLoot);
		LootDeck.Remove(RandomLoot);
	}
}

void UBBLootLibrary::AttachLootToPlayer(ELootType Type, class ABBPlayerCharacterBase* Character)
{
	Character->AddLootComponent(Type);
}

void UBBLootLibrary::DetachLootFromPlayer(ELootType Type, class ABBPlayerCharacterBase* Character)
{
	Character->RemoveLootComponent(Type);
}

void UBBLootLibrary::GiveLootToOtherPlayer(ELootType Type, class ABBPlayerCharacterBase* CharacterWithLoot, class ABBPlayerCharacterBase* OtherCharacter)
{
	if (!CharacterWithLoot->Loot.Contains(Type) || !CharacterWithLoot || !OtherCharacter)
	{
		return;
	}
	DetachLootFromPlayer(Type, CharacterWithLoot);
	AttachLootToPlayer(Type, OtherCharacter);
}

bool UBBLootLibrary::HasToolBeenDrawn(ELootType Type)
{
	return !LootDeck.Contains(Type);
}
