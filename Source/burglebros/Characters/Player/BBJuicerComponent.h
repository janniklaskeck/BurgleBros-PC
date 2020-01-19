// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BBPlayerClassComponent.h"
#include "BBJuicerComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBJuicerComponent : public UBBPlayerClassComponent
{
	GENERATED_BODY()
public:
	bool CanUseAbility() override;
	bool IsActiveAbility() override;
	void UseAbility() override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bHasAlarmToken = false;

	TArray<class ABBRoomBase*> GetValidRooms();
	UFUNCTION()
		void OnRoomChosen(class ABBRoomBase* ChosenRoom);
};
