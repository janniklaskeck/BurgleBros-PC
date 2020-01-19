// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBEventInfo.h"
#include "BBEventList.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class BURGLEBROS_API UBBEventList : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TArray<UBBEventInfo*> EventList;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UBBEventInfo* GetEventByType(EEventType Event);
};
