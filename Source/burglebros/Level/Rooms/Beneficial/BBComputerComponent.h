// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Rooms/BBRoomFunctionComponent.h"
#include "BBComputerComponent.generated.h"

UCLASS()
class BURGLEBROS_API UBBComputerComponent : public UBBRoomFunctionComponent
{
	GENERATED_BODY()
public:
	bool CanUse(class ABBCharacterBase* Character) override;
	void OnUsed(class ABBCharacterBase* Character) override;
	UFUNCTION(BlueprintCallable)
		void UseToken();

	UPROPERTY(BlueprintReadOnly)
		int32 AmountTokens = 0;
	UPROPERTY(BlueprintReadOnly)
		int32 MaxTokens = 6;

	UFUNCTION(BlueprintCallable)
		void AddToken(class ABBCharacterBase* Character);
	void AddTokens(int32 Amount);
	UFUNCTION(BlueprintCallable)
		bool CanAddToken(class ABBCharacterBase* Character);
};
