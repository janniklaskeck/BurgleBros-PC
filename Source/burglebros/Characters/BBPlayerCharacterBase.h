// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BBCharacterBase.h"
#include "BBRoomPosition.h"
#include "BBToolInfo.h"
#include "BBLootInfo.h"
#include "BBPlayerClassComponent.h"
#include "BBPlayerClass.h"
#include "BBLootComponent.h"
#include "BBPlayerCharacterBase.generated.h"

UCLASS()
class BURGLEBROS_API ABBPlayerCharacterBase : public ABBCharacterBase
{
	GENERATED_BODY()
public:
	void BeginPlay() override;
	void StartTurn() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Class)
		EPlayerClass PlayerType;
protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Class, meta = (AllowPrivateAccess = "true"))
		class UBBPlayerClassComponent* ClassComponent;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Class, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UBBPlayerClassComponent> PlayerClass;
public:
	UFUNCTION(BlueprintCallable)
		class UBBPlayerClassComponent* GetClassComponent() const;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int32 StealthTokens = 3;
public:
	void InitCharacter(class ABBLevel* Level, FRoomPosition InitialRoom) override;
	void InitPlayerClass(EPlayerClass ActualPlayerClass);
	int32 GetStealthTokens();
	void AddStealthToken();
	void ConsumeStealthToken();

	UPROPERTY(BlueprintReadOnly)
		TArray<EToolType> Tools;

	UPROPERTY(BlueprintReadOnly)
		TArray<ELootType> Loot;

	void AddLootComponent(ELootType Type);
	void RemoveLootComponent(ELootType Type);
	UPROPERTY(BlueprintAssignable)
		FOnAction OnLootChanged;

	UPROPERTY(BlueprintReadOnly)
		bool bIsInvisible = false;
	UPROPERTY(BlueprintReadOnly)
		bool bIsEscaped = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool CanMoveToSelectedRoom();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool CanMoveToUpperRoom();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool CanMoveToLowerRoom();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool CanPeekSelectedRoom();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool CanPeekUpperRoom();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool CanPeekLowerRoom();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool CanUseSelectedRoom();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool CanUseTool();
private:
	bool CanMoveToRoof();
	void MoveToRoof();
public:
	UFUNCTION(BlueprintCallable)
		void MoveToSelectedRoom();
	UFUNCTION(BlueprintCallable)
		void MoveToLowerRoom();
	UFUNCTION(BlueprintCallable)
		void MoveToUpperRoom();
	UFUNCTION(BlueprintCallable)
		void PeekSelectedRoom();
	UFUNCTION(BlueprintCallable)
		void PeekUpperRoom();
	UFUNCTION(BlueprintCallable)
		void PeekLowerRoom();
	UFUNCTION(BlueprintCallable)
		void UseSelectedRoom();
	UFUNCTION(BlueprintCallable)
		void UseCharacterAbility();
};
