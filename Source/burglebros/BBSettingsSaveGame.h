// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BBSettingsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class BURGLEBROS_API UBBSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		float MasterVolume = .5f;
	UPROPERTY(EditDefaultsOnly)
		float EffectsVolume = .5f;
	UPROPERTY(EditDefaultsOnly)
		float MusicVolume = .5f;

	UPROPERTY(EditDefaultsOnly)
		float CameraSpeed = 1.f;
};
