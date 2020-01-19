// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBPlayerCharacterBase.h"
#include "BBPlayerClassInfo.generated.h"

UCLASS(BlueprintType)
class BURGLEBROS_API UBBPlayerClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FText Name;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FText Title;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FText AbilityName;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (MultiLine = "true"))
		FText AbilityDescription;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FText AbilityNameAlt;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (MultiLine = "true"))
		FText AbilityDescriptionAlt;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		EPlayerClass PlayerClass;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		EPlayerClass PlayerClassAlt;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<ABBPlayerCharacterBase> Class;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FLinearColor ClassColor;

	UFUNCTION(BlueprintCallable)
		FText GetClassAbilityName(EPlayerClass SelectedPlayerClass);
	UFUNCTION(BlueprintCallable)
		FText GetClassAbilityDescription(EPlayerClass SelectedPlayerClass);
};
