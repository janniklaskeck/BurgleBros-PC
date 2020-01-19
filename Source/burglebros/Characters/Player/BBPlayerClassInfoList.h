// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBPlayerClassInfo.h"
#include "BBPlayerClassInfoList.generated.h"

UCLASS(BlueprintType)
class BURGLEBROS_API UBBPlayerClassInfoList : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TArray<UBBPlayerClassInfo*> ClassInfoList;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UBBPlayerClassInfo* GetPlayerClassInfo(EPlayerClass PlayerClass);
};
