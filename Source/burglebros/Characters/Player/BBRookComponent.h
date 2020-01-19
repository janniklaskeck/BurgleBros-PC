// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BBPlayerClassComponent.h"
#include "BBRookComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBRookComponent : public UBBPlayerClassComponent
{
	GENERATED_BODY()
public:
	bool CanUseAbility() override;
	bool IsActiveAbility() override;

	void UseAbility() override;
private:
	UFUNCTION()
		void ActivateAbility(class ABBPlayerCharacterBase* Character);

	UFUNCTION()
		void SwitchPositionWith(class ABBPlayerCharacterBase* OtherCharacter);
	UFUNCTION()
		void StartToMoveOtherCharacter();
	TArray<class ABBRoomBase*> GetValidMoveOtherCharacterRooms();
	UFUNCTION()
		void OnMoveOtherCharacterRoomChosen(class ABBRoomBase* ChosenRoom);
};
