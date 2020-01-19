// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BBPlayerClassComponent.h"
#include "BBRiggerComponent.generated.h"

/**
 * 
 */
UCLASS()
class BURGLEBROS_API UBBRiggerComponent : public UBBPlayerClassComponent
{
	GENERATED_BODY()
public:
	void BeginPlay() override;

	bool CanUseAbility() override;
	bool IsActiveAbility() override;

	void UseAbility() override;
};
