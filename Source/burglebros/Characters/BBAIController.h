// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BBDelegates.h"
#include "BBAIController.generated.h"

UCLASS(Blueprintable)
class BURGLEBROS_API ABBAIController : public AAIController
{
	GENERATED_BODY()
public:

	void MoveToTarget(FVector Target, float AcceptanceRadius);

	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY()
		FOnAction OnMoveSuccess;
	UPROPERTY()
		FOnAction OnMoveFailed;
};
