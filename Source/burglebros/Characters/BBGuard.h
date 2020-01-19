// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBCharacterBase.h"
#include "BBRoomPosition.h"
#include "BBGuard.generated.h"

USTRUCT(BlueprintType)
struct FPatrolInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FRoomPosition TargetRoom;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<FRoomPosition> PathToTarget;
	UPROPERTY(BlueprintReadOnly)
		bool bIsAlarm = false;

	void UpdatePathToTarget(TArray<FRoomPosition> NewPathToTarget)
	{
		PathToTarget.Empty(16);
		PathToTarget.Append(NewPathToTarget);
		UE_LOG(LogTemp, Warning, TEXT("Update Patrol Path"));
	}

	void SetNewTarget(FRoomPosition NewTarget)
	{
		TargetRoom = NewTarget;
	}
};

UCLASS()
class BURGLEBROS_API ABBGuard : public ABBCharacterBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<FRoomPosition> PatrolDeck;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<FRoomPosition> VisitedRooms;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FPatrolInfo CurrentPatrolInfo;

	void BeginPlay() override;

	void OnRoomEntered(class ABBRoomBase* Room) override;
	FOnAction OnGuardMovementEnd;
	
	void StartTurn() override;
	void FinishTurn() override;

	void InitCharacter(class ABBLevel* Level, FRoomPosition InitialRoom) override;
	UFUNCTION(BlueprintCallable)
		void DrawNextPatrolTarget(bool AddToVisited = true, bool RemoveFromDeck = true);
	void DrawFirstPatrolTarget();
	void MoveTopPatrolTargetToBottom();
	UFUNCTION(BlueprintCallable)
		FRoomPosition GetNextPatrolTarget();
	void SetNewPatrolTarget(FRoomPosition Position);
	void UpdatePatrolPath();

	void SetCharacterHidden(bool IsHidden) override;
private:

	class ABBAIController* GetBBAIController() const;
	void InitPatrolDeck();
	void CheckPatrolDeck();
	TArray<FRoomPosition> GetPathToPatrolTarget();
public:
	FRoomPosition GetClosestRoomWithAlarm();

private:
	void OnMovementStarted();
	void OnMovementFinished();
	
	void UpdateGuardMovement();
	UFUNCTION()
		void OnTargetRoomAlarmChange(bool HasAlarm);
	FRoomPosition ReachableRoom;
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		bool bHasDonuts = false;
	UFUNCTION(BlueprintCallable)
		void GiveDonuts();
};
