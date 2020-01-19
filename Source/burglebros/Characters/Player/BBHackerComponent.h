// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BBPlayerClassComponent.h"
#include "BBHackerComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBHackerComponent : public UBBPlayerClassComponent
{
	GENERATED_BODY()
public:
	bool CanUseAbility() override;
	bool IsActiveAbility() override;

	void UseAbility() override;

	bool HasToken();

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bHasToken = false;
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void UseToken();
};
