// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBLootInfo.h"
#include "BBLootInfoList.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class BURGLEBROS_API UBBLootInfoList : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TArray<UBBLootInfo*> LootList;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UBBLootInfo* GetLootByType(ELootType Loot);
};
