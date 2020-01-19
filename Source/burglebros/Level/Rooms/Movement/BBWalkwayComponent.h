// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBWalkwayComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBWalkwayComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	bool OnEntered(class ABBCharacterBase* Character) override;

	bool CanUse(class ABBCharacterBase* Character) override;
	void OnUsed(class ABBCharacterBase* Character) override;

private:
	void MoveCharacterDown(class ABBCharacterBase* Character);
};
