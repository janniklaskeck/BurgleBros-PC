// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBSafeComponent.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBSafeComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:

	bool CanUse(class ABBCharacterBase* Character) override;
	void OnUsed(class ABBCharacterBase* Character) override;

	UPROPERTY(BlueprintReadOnly)
		int32 AmountDice = 0;
	UPROPERTY(BlueprintReadOnly)
		bool bIsCracked = false;

	UFUNCTION(BlueprintCallable)
		bool CanAddDie(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		bool CanThrowDice(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		void AddDie(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		void RollDice(class ABBCharacterBase* Character);
	UFUNCTION()
		void ApplyRollResults();

private:
	TArray<class ABBRoomBase*> GetNecessaryDiceResults(bool CheckForDiscovered);
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		TArray<int32> GetSafeCombination();
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (AllowPrivateAccess = "true"))
		bool IsNumberAlreadyCracked(int32 Number);
};
