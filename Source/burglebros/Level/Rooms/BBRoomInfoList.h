// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBRoomInfo.h"
#include "BBRoomPosition.h"
#include "BBRoomInfoList.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BURGLEBROS_API UBBRoomInfoList : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TArray<UBBRoomInfo*> RoomList;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UBBRoomInfo* GetRoomByType(ERoomType Room);
};
