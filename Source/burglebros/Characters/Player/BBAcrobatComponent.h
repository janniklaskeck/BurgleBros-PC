// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BBPlayerClassComponent.h"
#include "BBAcrobatComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBAcrobatComponent : public UBBPlayerClassComponent
{
	GENERATED_BODY()
public:
	bool CanUseAbility() override;
	bool IsActiveAbility() override;

	void UseAbility() override;

private:
	class ABBRoomBase* GetRoomWithGuard();

	class ABBRoomBase* GuardRoom;
	bool MovedAfterAction;
	UFUNCTION()
		void CheckForFurtherMovement(class ABBRoomBase* EnteredRoom);
	UFUNCTION()
		void CheckOnTurnEnd();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void ClimbUp();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void ClimbDown();
};
