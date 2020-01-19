// Fill out your copyright notice in the Description page of Project Settings.

#include "BBAIController.h"

void ABBAIController::MoveToTarget(FVector Target, float AcceptanceRadius)
{
	EPathFollowingRequestResult::Type PathResult = MoveToLocation(Target, AcceptanceRadius, true, true, false, true, NULL, false);
	if (PathResult == EPathFollowingRequestResult::Type::AlreadyAtGoal)
	{
		OnMoveSuccess.Broadcast();
	}
}

void ABBAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.Code == EPathFollowingResult::Type::Success)
	{
		OnMoveSuccess.Broadcast();
	}
	else if (Result.Code == EPathFollowingResult::Type::Blocked || Result.Code == EPathFollowingResult::Type::Invalid)
	{
		OnMoveFailed.Broadcast();
	}
}
