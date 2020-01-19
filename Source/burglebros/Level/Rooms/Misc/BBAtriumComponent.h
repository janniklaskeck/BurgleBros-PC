// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBAtriumComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBAtriumComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	void BeginPlay() override;
	bool OnEntered(class ABBCharacterBase* Character) override;

	bool CanUse(class ABBCharacterBase* Character) override;
	void OnUsed(class ABBCharacterBase* Character) override;

	UFUNCTION(BlueprintCallable)
		bool CanPeekAbove(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		void PeekAbove(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		bool CanPeekBelow(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		void PeekBelow(class ABBCharacterBase* Character);
private:
	class ABBRoomBase* GetRoomAbove();
	class ABBRoomBase* GetRoomBelow();

	UFUNCTION()
		void BindToNeighbourRooms();
	UFUNCTION()
		void CheckForGuard(class ABBCharacterBase* Character);
};
