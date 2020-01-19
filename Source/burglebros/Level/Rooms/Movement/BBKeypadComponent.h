// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBKeypadComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBKeypadComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	void BeginPlay() override;
	bool CanEnter(class ABBCharacterBase* Character) override;
	bool OnEntered(class ABBCharacterBase* Character) override;
	bool CanDisableRoom() override;

	bool bIsUnlocked = false;

private:
	int32 AmountTries = 0;
	UFUNCTION()
		void ResetTries();
	UFUNCTION()
		void CheckResults();
};
