// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBLootInfo.generated.h"

UENUM(BlueprintType)
enum class ELootType : uint8
{
	Isotope,
	Keycard,
	Mirror,
	Chihuahua,
	Bust,
	Stamp,
	PersianKitty,
	Tiara,
	Gemstone,
	GoldBar,
	CursedGoblet,
	Painting
};


UCLASS(BlueprintType)
class BURGLEBROS_API UBBLootInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FText Title;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (MultiLine = "true"))
		FText Description;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		ELootType Type;
};
