// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BBPlayerClass.h"
#include "BBGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FPlayerClass
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		EPlayerClass PlayerClass;
	UPROPERTY(BlueprintReadWrite)
		bool bIsAlternativeClass = false;
	UPROPERTY(BlueprintReadWrite)
		bool bIsEnabled = false;
};

UCLASS()
class BURGLEBROS_API UBBGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	void Init() override;
	void Shutdown() override;
public:
	UFUNCTION(BlueprintCallable)
		void IncreaseCameraSpeed(float Interval = 0.2f);
	UFUNCTION(BlueprintCallable)
		void DecreaseCameraSpeed(float Interval = -.2f);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetCameraSpeedScale();

	UFUNCTION(BlueprintCallable)
		void SaveSettings();
	void LoadSettings();
private:
	UPROPERTY(EditDefaultsOnly)
		class USoundClass* MasterClass;
	UPROPERTY(EditDefaultsOnly)
		class USoundClass* EffectsClass;
	UPROPERTY(EditDefaultsOnly)
		class USoundClass* MusicClass;

	UPROPERTY()
		class UBBSettingsSaveGame* SettingsSaveGame;
	FString SettingsSaveGameName = FString(TEXT("SettingsSlot"));

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<FPlayerClass> GetSelectedPlayerClasses() const;
private:
	UPROPERTY()
		TArray<FPlayerClass> SelectedPlayerClasses;
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (AllowPrivateAccess = "true"))
		bool CanAddClass(EPlayerClass PlayerClass);
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void SetClass(EPlayerClass Class, bool IsAlternative, int32 Index);
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void RemoveClass(int32 Index);
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void ResetClasses();
};
