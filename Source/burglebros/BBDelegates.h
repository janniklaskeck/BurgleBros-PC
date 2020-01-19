// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/Actor.h"
#include "BBDelegates.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterAction, class ABBCharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCharacterAction, class ABBPlayerCharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomAction, class ABBRoomBase*, ChosenRoom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntChange, int32, IntValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoolChange, bool, BoolValue);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRoomAction_NonDynamic, class ABBMainGameMode*, class ABBRoomBase*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterAction_NonDynamic, class ABBPlayerCharacterBase*);

UCLASS()
class BURGLEBROS_API ABBDelegates : public AActor
{
	GENERATED_BODY()
};