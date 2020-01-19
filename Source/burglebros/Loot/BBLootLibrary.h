// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBLootInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BBLootLibrary.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBLootLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
private:
	static TArray<ELootType> LootDeck;
public:
	static void InitLootDeck();

	static void DrawRandomLoot(class ABBPlayerCharacterBase* Character);
	static void DrawRandomLoot(class ABBRoomBase* Room);
	static void AttachLootToPlayer(ELootType Type, class ABBPlayerCharacterBase* Character);
	static void DetachLootFromPlayer(ELootType Type, class ABBPlayerCharacterBase* Character);
	static void GiveLootToOtherPlayer(ELootType Type, class ABBPlayerCharacterBase* CharacterWithLoot, class ABBPlayerCharacterBase* OtherCharacter);
	static bool HasToolBeenDrawn(ELootType Type);
};
