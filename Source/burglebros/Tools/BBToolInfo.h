// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBToolInfo.generated.h"

UENUM(BlueprintType)
enum class EToolType : uint8
{
	CrystalBall = 0,
	Virus,
	RollerSkates,
	EMP,
	Dynamite,
	ThermalBomb,
	SmokeBomb,
	InvisibleSuit,
	Blueprints,
	Donuts,
	MakupKit,
	Crowbar,
	Stethoscope
};

UCLASS(BlueprintType)
class BURGLEBROS_API UBBToolInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText Title;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = "true"))
		FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EToolType Type;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsActiveUse = true;
};
