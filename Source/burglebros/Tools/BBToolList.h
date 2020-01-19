// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBToolInfo.h"
#include "Engine/DataAsset.h"
#include "BBToolList.generated.h"

UCLASS(BlueprintType)
class BURGLEBROS_API UBBToolList : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TArray<UBBToolInfo*> ToolList;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UBBToolInfo* GetToolByType(EToolType Tool);
};
