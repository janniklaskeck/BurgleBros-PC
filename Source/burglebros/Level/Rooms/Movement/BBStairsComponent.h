// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBStairsComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBStairsComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	bool CanUse(class ABBCharacterBase* Character) override;
	void OnUsed(class ABBCharacterBase* Character) override;

	bool CanDisableRoom() override;
};
