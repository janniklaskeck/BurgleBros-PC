// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBLaserComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBLaserComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	bool OnEntered(class ABBCharacterBase* Character) override;
	bool CanDisableRoom() override;

	UFUNCTION(BlueprintCallable)
		void EnterWithOneAction(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		void EnterWithTwoAction(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		bool CanEnterWithTwoActions(class ABBCharacterBase* Character);
};
