// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBEventLibrary.h"
#include "BBEventComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BURGLEBROS_API UBBEventComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	void InitEvent(EEventType Type);

private:
	EEventType Type;
	UFUNCTION()
		void DisableTimeLock();
	UFUNCTION()
		void DisableGymnastics();
	UFUNCTION()
		void DisableVideoLoop();
	class ABBPlayerCharacterBase* GetOwnerCharacter();
};
