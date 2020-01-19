// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBFoyerComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBFoyerComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	void BeginPlay() override;
	bool OnEntered(class ABBCharacterBase* Character) override;
private:
	UFUNCTION()
		void BindToNeighbourRooms();
	UFUNCTION()
		void CheckForGuard(class ABBCharacterBase* CharacterEntered);
};
