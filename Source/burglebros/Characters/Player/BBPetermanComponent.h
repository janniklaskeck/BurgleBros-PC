// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BBPlayerClassComponent.h"
#include "BBPetermanComponent.generated.h"

/**
 * 
 */
UCLASS()
class BURGLEBROS_API UBBPetermanComponent : public UBBPlayerClassComponent
{
	GENERATED_BODY()
public:
	bool CanUseAbility() override;
	bool IsActiveAbility() override;

	void UseAbility() override;
private:
	class ABBRoomBase* GetBelowSafeRoom();
	class ABBRoomBase* GetAboveSafeRoom();
};
