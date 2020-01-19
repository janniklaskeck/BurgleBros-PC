// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBCameraComponent.generated.h"

UCLASS()
class BURGLEBROS_API UBBCameraComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	bool OnEntered(class ABBCharacterBase* Character) override;
	bool CanDisableRoom() override;
};
