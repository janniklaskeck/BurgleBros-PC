// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBRoomFunctionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Abstract)
class BURGLEBROS_API UBBRoomFunctionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBBRoomFunctionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	bool bIsRoomEnabled = true;
	virtual bool CanDisableRoom();

	bool OnEnter(class ABBCharacterBase* Character, bool IgnoreEffects = false, bool IgnoreCost = false);
	void OnLeave(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		void OnUse(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		void PeekRoom(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		bool CanPeekRoom(class ABBCharacterBase* Character, bool IgnoreFloors = false);
	virtual int32 GetEnterCost(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		virtual bool CanEnter(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		virtual bool CanUse(class ABBCharacterBase* Character);
protected:
	virtual bool OnEntered(class ABBCharacterBase* Character);
	virtual void OnLeft(class ABBCharacterBase* Character);

	virtual void OnUsed(class ABBCharacterBase* Character);

public:
	UFUNCTION(BlueprintCallable)
		class ABBRoomBase* GetOwnerRoom();
protected:
	bool IsGuard(class ABBCharacterBase* Character);
};
