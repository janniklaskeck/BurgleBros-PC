// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBPlayerClass.h"
#include "BBPlayerClassComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BURGLEBROS_API UBBPlayerClassComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBBPlayerClassComponent();

protected:
	virtual void BeginPlay() override;

	EPlayerClass PlayerClass;

	class ABBPlayerCharacterBase* GetOwnerCharacter();
	bool bUsedAbility = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual bool CanUseAbility();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual bool IsActiveAbility();
	UFUNCTION(BlueprintCallable)
		virtual void UseAbility();
private:
	UFUNCTION()
		void ResetUsed();
};
