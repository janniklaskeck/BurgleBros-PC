// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BBPlayerClassComponent.h"
#include "BBSpotterComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBSpotterComponent : public UBBPlayerClassComponent
{
	GENERATED_BODY()
public:
	bool CanUseAbility() override;
	bool IsActiveAbility() override;

	void UseAbility() override;
private:
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void MoveTopEventToBottom();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void MoveTopPatrolToBottom();
};
