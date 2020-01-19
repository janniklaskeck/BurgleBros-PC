// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBEventInfo.generated.h"

UENUM(BlueprintType)
enum class EEventType : uint8
{
	DeadDrop,
	SwitchSigns,
	JumpTheGun,
	GoWithYourGut,
	WhereIsHe,
	ChangeOfPlans,
	Squeak,
	ShiftChange,
	Peekhole,
	Espresso,
	Reboot,
	KeycodeChange,
	BrownOut,
	Daydreaming,
	FreightElevator,
	LostGrip,
	Jury_Rig,
	ThrowVoice,
	VideoLoop,
	HeadsUp,
	TimeLock,
	BuddySystem,
	Shoplifting,
	Gymnastics,
	Lampshade,
	Crash
};


UCLASS(BlueprintType)
class BURGLEBROS_API UBBEventInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText Title;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = "true"))
		FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EEventType Type;
};
