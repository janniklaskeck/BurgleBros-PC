// Fill out your copyright notice in the Description page of Project Settings.

#include "BBGameInstance.h"
#include "BBUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "BBSettingsSaveGame.h"
#include "Sound/SoundClass.h"

void UBBGameInstance::Init()
{
	Super::Init();
	UBBUtilities::InitRandomStream();
	ResetClasses();
	LoadSettings();
}

void UBBGameInstance::Shutdown()
{
	Super::Shutdown();
	SaveSettings();
}

void UBBGameInstance::IncreaseCameraSpeed(float Interval /*= 0.2f*/)
{
	SettingsSaveGame->CameraSpeed = FMath::Clamp(SettingsSaveGame->CameraSpeed + Interval, 0.2f, 2.f);
}

void UBBGameInstance::DecreaseCameraSpeed(float Interval /*= -.2f*/)
{
	SettingsSaveGame->CameraSpeed = FMath::Clamp(SettingsSaveGame->CameraSpeed + Interval, 0.2f, 2.f);
}

float UBBGameInstance::GetCameraSpeedScale()
{
	return SettingsSaveGame->CameraSpeed;
}

void UBBGameInstance::SaveSettings()
{
	if (!SettingsSaveGame)
	{
		SettingsSaveGame = Cast<UBBSettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(UBBSettingsSaveGame::StaticClass()));
	}
	if (MasterClass)
		SettingsSaveGame->MasterVolume = MasterClass->Properties.Volume;
	if (EffectsClass)
		SettingsSaveGame->EffectsVolume = EffectsClass->Properties.Volume;
	if (MusicClass)
		SettingsSaveGame->MusicVolume = MusicClass->Properties.Volume;
	UGameplayStatics::SaveGameToSlot(SettingsSaveGame, SettingsSaveGameName, 0);
}

void UBBGameInstance::LoadSettings()
{
	USaveGame* LoadedSave = UGameplayStatics::LoadGameFromSlot(SettingsSaveGameName, 0);
	if (LoadedSave)
	{
		SettingsSaveGame = Cast<UBBSettingsSaveGame>(LoadedSave);
		if (MasterClass)
			MasterClass->Properties.Volume = SettingsSaveGame->MasterVolume;
		if (EffectsClass)
			EffectsClass->Properties.Volume = SettingsSaveGame->EffectsVolume;
		if (MusicClass)
			MusicClass->Properties.Volume = SettingsSaveGame->MusicVolume;
	}
	else
	{
		SaveSettings();
	}
}

TArray<FPlayerClass> UBBGameInstance::GetSelectedPlayerClasses() const
{
	return SelectedPlayerClasses;
}

bool UBBGameInstance::CanAddClass(EPlayerClass PlayerClass)
{
	return !SelectedPlayerClasses.ContainsByPredicate([PlayerClass](FPlayerClass ClassStruct)
	{
		return ClassStruct.PlayerClass == PlayerClass && ClassStruct.bIsEnabled;
	});
}

void UBBGameInstance::SetClass(EPlayerClass Class, bool IsAlternative, int32 Index)
{
	SelectedPlayerClasses[Index].bIsEnabled = true;
	SelectedPlayerClasses[Index].PlayerClass = Class;
	SelectedPlayerClasses[Index].bIsAlternativeClass = IsAlternative;
}

void UBBGameInstance::RemoveClass(int32 Index)
{
	SelectedPlayerClasses[Index].bIsEnabled = false;
}

void UBBGameInstance::ResetClasses()
{
	SelectedPlayerClasses.Reset(4);
	SelectedPlayerClasses.Add(FPlayerClass());
	SelectedPlayerClasses.Add(FPlayerClass());
	SelectedPlayerClasses.Add(FPlayerClass());
	SelectedPlayerClasses.Add(FPlayerClass());
}
