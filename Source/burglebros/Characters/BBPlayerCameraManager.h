// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "BBPlayerCameraManager.generated.h"

UCLASS()
class BURGLEBROS_API ABBPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
protected:
	void BeginPlay() override;
	void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
private:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float MinCameraDistance = 500.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float MaxCameraDistance = 500.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float DefaultFOVValue = 90.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float DefaultRotationPitch = 70.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float DefaultRotationYaw = 0.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float DefaultRotationRoll = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = PostProcessEffects)
		FPostProcessSettings Settings;

	FRotator GetDefaultRotation()
	{
		return FRotator(DefaultRotationPitch, DefaultRotationYaw, DefaultRotationRoll);
	}


	FMinimalViewInfo GetPOV(float DeltaTime);
	AActor* GetCurrentViewTarget();
	FVector GetCameraLocation(float DeltaTime);
public:
	UFUNCTION(BlueprintCallable)
		void FocusOnViewTarget();
	void SetCustomViewtarget(FVector TargetLocation, int32 TargetFloor);
private:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float LerpSpeed = 100.f;
	FVector CurrentTarget = FVector::ZeroVector;
	FVector CurrentPosition = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float MoveSpeed = 50.f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int32 CurrentFloor = 0;
	UFUNCTION()
		void OnEnterPlayerOrGuardTurn();
	void MoveToFloor(int32 Floor);
private:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		float ZoomSpeed = 50.f;
	float CurrentZoom;
public:
	void Zoom(float AxisValue);
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void MoveUpFloor();
	void MoveDownFloor();
};
