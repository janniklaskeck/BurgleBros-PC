// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BBPlayerClassComponent.h"
#include "BBHawkComponent.generated.h"

/**
 * 
 */
UCLASS()
class BURGLEBROS_API UBBHawkComponent : public UBBPlayerClassComponent
{
	GENERATED_BODY()
public:
	bool CanUseAbility() override;
	bool IsActiveAbility() override;
	void UseAbility() override;
private:
	TArray<class ABBRoomBase*> GetValidThroughWallRooms();
	TArray<class ABBRoomBase*> GetValidTwoDistanceRooms();

	UFUNCTION()
		void OnValidRoomChosen(class ABBRoomBase* ChosenRoom);
};
