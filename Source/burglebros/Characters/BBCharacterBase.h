// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BBRoomPosition.h"
#include "BBDelegates.h"
#include "BBCharacterBase.generated.h"

USTRUCT(BlueprintType)
struct FCharacterActions
{
	GENERATED_BODY()
private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		int32 MaxActions = 4;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		int32 CurrentActions = 4;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		int32 EventThreshold = 2;
public:
	UPROPERTY(BlueprintAssignable)
		FOnAction OnActionsChange;
	void Reset()
	{
		CurrentActions = MaxActions;
		OnActionsChange.Broadcast();
	}
	bool HasActionsLeft()
	{
		return CurrentActions > 0;
	}
	void ConsumeAction()
	{
		if (CurrentActions > 0)
		{
			--CurrentActions;
			OnActionsChange.Broadcast();
		}
	}
	void ConsumeActions(int32 Amount)
	{
		FMath::Clamp(CurrentActions -= Amount, 0, 100);
		OnActionsChange.Broadcast();
	}
	void AddOneAction()
	{
		++CurrentActions;
		OnActionsChange.Broadcast();
	}
	void SetCurrentActions(int32 Amount)
	{
		CurrentActions = Amount;
		OnActionsChange.Broadcast();
	}
	int32 GetCurrentActions()
	{
		return CurrentActions;
	}
	int32 GetMaxActions()
	{
		return MaxActions;
	}
	void SetMaxActions(int32 NewMaxActions)
	{
		MaxActions = FMath::Clamp(NewMaxActions, 0, 6);
		if (MaxActions < CurrentActions)
		{
			CurrentActions = MaxActions;
		}
		OnActionsChange.Broadcast();
	}
	void SetEventThreshold(int32 NewEventThreshold)
	{
		EventThreshold = NewEventThreshold;
		OnActionsChange.Broadcast();
	}
	bool IsPastEventThreshold()
	{
		return CurrentActions >= EventThreshold;
	}
};

class ABBLevel;
class ABBRoomBase;

UCLASS()
class BURGLEBROS_API ABBCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABBCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Category = Navigation, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UBBLevelNavigationComponent* LevelNavigationComponent;
	UPROPERTY(Category = Navigation, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SelectionStaticMeshComponent;
public:
	TArray<FRoomPosition> GetPath(FRoomPosition& Start, FRoomPosition& End);
	TArray<FRoomPosition> GetPath(FRoomPosition& Target);
	UFUNCTION(BlueprintCallable)
		class UBBLevelNavigationComponent* GetLevelNavigationComponent() const;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		ABBLevel* CurrentLevel;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		ABBRoomBase* PreviousRoom;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		ABBRoomBase* CurrentRoom;

	virtual void InitCharacter(ABBLevel* CurrentLevel, FRoomPosition InitialRoom);

	UFUNCTION()
		virtual void OnRoomEntered(ABBRoomBase* Room);
	UFUNCTION()
		virtual void OnRoomLeft(ABBRoomBase* Room);

	UPROPERTY(BlueprintAssignable)
		FOnRoomAction DOnRoomEntered;
	UPROPERTY(BlueprintAssignable)
		FOnRoomAction DOnRoomLeft;

	virtual void StartTurn();
	virtual void FinishTurn();

	UPROPERTY(BlueprintAssignable)
		FOnAction OnTurnStart;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FCharacterActions Actions;
	UPROPERTY(BlueprintAssignable)
		FOnAction OnActionsChange;
	UFUNCTION()
		void OnActionsChangedForwarder();

	virtual void SetCharacterHidden(bool IsHidden);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool HasActionsLeft()
	{
		return Actions.HasActionsLeft();
	}
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetCurrentActions()
	{
		return Actions.GetCurrentActions();
	}
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetMaxActions()
	{
		return Actions.GetMaxActions();
	}
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsPastEventThreshold()
	{
		return Actions.IsPastEventThreshold();
	}
};
