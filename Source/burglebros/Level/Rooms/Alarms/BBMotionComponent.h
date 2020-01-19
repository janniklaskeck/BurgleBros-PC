// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBMotionComponent.generated.h"

UCLASS()
class BURGLEBROS_API UBBMotionComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	void BeginPlay() override;
	bool OnEntered(class ABBCharacterBase* Character) override;
	void OnLeft(class ABBCharacterBase* Character) override;
	bool CanDisableRoom() override;
private:
	TArray<class ABBCharacterBase*> EnteredCharacters;
	UFUNCTION()
		void OnTurnEnd();
};
