// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBRoomPosition.h"
#include "BBRoomInfo.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class BURGLEBROS_API UBBRoomInfo : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FText Title;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FText Description;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		ERoomType Type;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FLinearColor DiscoveredColor;
};
