// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBServiceDuctComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBServiceDuctComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	bool CanUse(class ABBCharacterBase* Character) override;
	void OnUsed(class ABBCharacterBase* Character) override;
private:
	UBBRoomFunctionComponent* GetOtherServiceDuct();
};
