// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBRoomPosition.h"
#include "BBToolInfo.h"
#include "BBPlayerController.generated.h"

class ABBRoomBase;
class ABBAIController;

UCLASS(Blueprintable)
class BURGLEBROS_API ABBPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ABBPlayerController();

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	void SetupInputComponent() override;

	void SetControlledCharacter(ABBAIController* CharacterController)
	{
		this->ControlledCharacterAIController = CharacterController;
	}

private:
	void CheckHovered();
	UPROPERTY()
		ABBAIController* ControlledCharacterAIController;

private:
	void ClickRoom();
	void DeselectRoom();
	void MoveUpFloor();
	void MoveDownFloor();

	void PauseGame();

	void CameraZoom(float Delta);
	void MoveForward(float Delta);
	void MoveRight(float Delta);

	void MouseForward(float Delta);
	void MouseRight(float Delta);
public:
	UFUNCTION(BlueprintCallable)
		class ABBCharacterBase* GetControlledCharacter();

private:
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void GetTool(TArray<EToolType> Types);
	
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void AddLoot(TArray<ELootType> Types);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void AddComputerTokens(int32 Amount);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void ResetActions();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void ResetStealth();
};
