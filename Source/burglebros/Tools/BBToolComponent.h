// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBToolInfo.h"
#include "BBToolComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BURGLEBROS_API UBBToolComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	void InitTool(EToolType Type);

private:
	EToolType Type;
	UFUNCTION()
		void DisableEMP();
	class ABBPlayerCharacterBase* GetOwnerCharacter();
};
