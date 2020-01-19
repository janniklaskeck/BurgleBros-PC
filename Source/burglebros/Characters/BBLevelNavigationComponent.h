// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBRoomPosition.h"
#include "BBDelegates.h"
#include "BBLevelNavigationComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLevelNavigation, Log, All);

USTRUCT()
struct FCell
{
	GENERATED_BODY()
public:
	FRoomPosition Parent = FRoomPosition(-1, -1, -1);
	float F = TNumericLimits<float>::Max();
	float G = TNumericLimits<float>::Max();
	float H = TNumericLimits<float>::Max();

	bool IsFMax()
	{
		return F == TNumericLimits<float>::Max();
	}
};

template<typename FirstType, typename SecondType>
struct FPair
{
	FirstType First;
	SecondType Second;

	FPair(FirstType FirstValue, SecondType SecondValue)
		: First(FirstValue), Second(SecondValue)
	{
	}
};

typedef FPair<float, FRoomPosition> PPair;

inline bool operator==(const PPair& Lhs, const PPair& Rhs)
{
	return Lhs.First == Rhs.First && Lhs.Second.PosX == Rhs.Second.PosX
		&& Lhs.Second.PosY == Rhs.Second.PosY
		&& Lhs.Second.Floor == Rhs.Second.Floor;
}

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BURGLEBROS_API UBBLevelNavigationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBBLevelNavigationComponent();

public:
	UFUNCTION(BlueprintCallable)
		bool CanMoveToRoom(class ABBRoomBase* Room);
	UFUNCTION(BlueprintCallable)
		bool CanMoveToRoomPosition(const FRoomPosition& RoomPosition);
	UFUNCTION(BlueprintCallable)
		void MoveToRoom(class ABBRoomBase* Room, bool IgnoreEffects = false, bool IgnoreCost = false);
	UFUNCTION(BlueprintCallable)
		void MoveToRoomPosition(const FRoomPosition& RoomPosition, bool IgnoreEffects = false, bool IgnoreCost = false);

	FOnRoomAction OnRoomEntered;
	FOnRoomAction OnRoomBlocked;
	FOnRoomAction OnFinalRoomEntered;

	float AcceptanceRadius = 10.0f;
	bool IsMoving();
private:
	bool bIsMoving;
	UFUNCTION()
		void OnRoomReached();
	bool IgnoreCostOnNextRoom = false;
	bool IgnoreEffectsOnNextRoom = false;
	TArray<FRoomPosition> CurrentMovementPath;
public:
	TArray<FRoomPosition> GetPath(class ABBRoomBase* Start, class ABBRoomBase* End);
	TArray<FRoomPosition> GetPath(FRoomPosition& Start, FRoomPosition& End);
private:
	class ABBLevel* GetCurrentLevel();

	bool IsValidRoom(int32 PosX, int32 PosY);
	bool IsValidRoom(FRoomPosition& RoomPosition);
	bool IsDirectionBlocked(FRoomPosition& RoomPosition, EDirection Direction);
	TArray<FRoomPosition> ConvertPath(TArray<TArray<FCell>> Cells, FRoomPosition& Target);
	float CalculateHeuristic(FRoomPosition& RoomPosition, FRoomPosition& Target);

private:
	bool IsGuard();
	class ABBCharacterBase* GetOwnerCharacter();
	class ABBAIController* GetOwnerAIController();
};
