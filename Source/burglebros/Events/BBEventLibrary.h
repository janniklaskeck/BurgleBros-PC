// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBEventInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BBEventLibrary.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBEventLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
private:
	static TArray<EEventType> EventDeck;
	static EEventType LastEvent;
public:
	static TArray<EEventType> ActiveEvents;

	static void InitEventDeck();
	UFUNCTION(BlueprintCallable)
		static void ExecuteEvent(EEventType Type, class ABBCharacterBase* Character, bool RemoveFromDeck = true);

	UFUNCTION(BlueprintCallable)
		static TArray<EEventType> GetFirstThreeEvents();
	UFUNCTION(BlueprintCallable)
		static void InsertEventsOnTop(TArray<EEventType> Events, bool RemoveDuplicatesFirst = true);
	UFUNCTION(BlueprintCallable)
		static void MoveFirstEventToBottom();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static EEventType GetTopEvent();

	static EEventType GetLastEvent();

private:
	static void ExecuteWhereIsHe(class ABBCharacterBase* Character);
	static void ExecuteEspresso(class ABBCharacterBase* Character);
	static void ExecuteReboot(class ABBCharacterBase* Character);
	static void ExecuteKeycodeChange(class ABBCharacterBase* Character);
	static void ExecuteDaydreaming(class ABBCharacterBase* Character);
	static void ExecuteFreightElevator(class ABBCharacterBase* Character);
	static void ExecuteLostGrip(class ABBCharacterBase* Character);
	static void ExecuteJuryRig(class ABBCharacterBase* Character);
	static void ExecuteShoplifting(class ABBCharacterBase* Character);
	static void ExecuteLampshade(class ABBCharacterBase* Character);
	static void ExecuteCrash(class ABBCharacterBase* Character);
	static void ExecuteDeadDrop(class ABBCharacterBase* Character);
	static void ExecuteSwitchSigns(class ABBCharacterBase* Character);
	static void ExecuteBrownOut(class ABBCharacterBase* Character);
	static void ExecuteChangeOfPlans(class ABBCharacterBase* Character);
	static void ExecuteVideoLoop(class ABBCharacterBase* Character);

	static void ExecuteHeadsUp(class ABBCharacterBase* Character);
	static void GiveHeadsUpAction(class ABBPlayerCharacterBase* Character);
	static FDelegateHandle HeadsUpDelegateHandle;

	static void ExecuteSqueak(class ABBCharacterBase* Character);

	static void ExecuteThrowVoice(class ABBCharacterBase* Character);
	static TArray<class ABBRoomBase*> GetValidThrowVoiceRooms(class ABBCharacterBase* Character);
	static void OnThrowVoiceRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom);

	static void ExecuteJumpTheGun(class ABBCharacterBase* Character);

	static void ExecuteGoWithYourGut(class ABBCharacterBase* Character);
	static TArray<class ABBRoomBase*> GetValidGoWithYourGutRooms(class ABBCharacterBase* Character);
	static void OnGoWithYourGutRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom);

	static void ExecuteShiftChange(class ABBCharacterBase* Character);
	static void ExecuteTimeLock(class ABBCharacterBase* Character);
	static void ExecuteGymnastics(class ABBCharacterBase* Character);

	static void ExecutePeekhole(class ABBCharacterBase* Character);
	static TArray<class ABBRoomBase*> GetValidPeekholeRooms(class ABBCharacterBase* Character);
	static void OnPeekholeRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom);

	static void ExecuteBuddySystem(class ABBCharacterBase* Character);
	static void OnBuddySystemCharacterChosen(class ABBPlayerCharacterBase* Character);

	static void EndEventCheck(UObject* WorldContext);
};
