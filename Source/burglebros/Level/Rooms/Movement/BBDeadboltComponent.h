// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBDeadboltComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBDeadboltComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	virtual int32 GetEnterCost(class ABBCharacterBase* Character) override;
	virtual bool CanEnter(class ABBCharacterBase* Character) override;
	bool CanDisableRoom() override;
};
