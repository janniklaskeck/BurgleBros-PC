// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BBToolInfo.h"
#include "BBToolLibrary.generated.h"

/**
 *
 */
UCLASS()
class BURGLEBROS_API UBBToolLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static TArray<EToolType> ActiveTools;

	static void InitDeck();
	UFUNCTION(BlueprintCallable)
		static void UseTool(class ABBPlayerCharacterBase* Character, EToolType ToolType);
	UFUNCTION(BlueprintCallable)
		static EToolType DrawRandomToolFromDeck(class ABBPlayerCharacterBase* PlayerCharacter, bool AddToPlayer = true);
	static EToolType DrawRandomToolFromDeck(class ABBRoomBase* Room);
	static bool GetToolFromDeck(EToolType ToolType);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static TEnumAsByte<EToolType> GetLastUsedTool();

	UFUNCTION(BlueprintCallable)
		static void RemoveToolFromCharacter(EToolType Tool, class ABBPlayerCharacterBase* Character);
private:
	static TArray<EToolType> ToolDeck;
	static EToolType LastUsedTool;

	static void UseBlueprints(class ABBPlayerCharacterBase* Character);
	static TArray<class ABBRoomBase*> GetValidBlueprintRooms(class ABBPlayerCharacterBase* Character);
	static void OnBlueprintRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom);

	static void UseCrowbar(class ABBPlayerCharacterBase* Character);
	static TArray<class ABBRoomBase*> GetValidCrowBarRooms(class ABBPlayerCharacterBase* Character);
	static void OnCrowBarRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom);

	static void UseCrystalBall(class ABBPlayerCharacterBase* Character);
	static void UseDonuts(class ABBPlayerCharacterBase* Character);
	static void UseDynamite(class ABBPlayerCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		static bool CanUseDynamite(class ABBRoomBase* Room, EDirection WallSide);
	UFUNCTION(BlueprintCallable)
		static void RemoveDynamiteWall(class ABBRoomBase* Room, EDirection WallSide);
	static void UseEMP(class ABBPlayerCharacterBase* Character);
	static void UseInvisibleSuit(class ABBPlayerCharacterBase* Character);
	static void UseMakeupKit(class ABBPlayerCharacterBase* Character);
	static void UseRollerSkates(class ABBPlayerCharacterBase* Character);
	static void UseSmokeBomb(class ABBPlayerCharacterBase* Character);
	static void UseStethoscope(class ABBPlayerCharacterBase* Character);
	static void UseThermalBomb(class ABBPlayerCharacterBase* Character);
	UFUNCTION(BlueprintCallable)
		static bool CanUseThermalBomb(class ABBRoomBase* Room, bool UseUp);
	UFUNCTION(BlueprintCallable)
		static void MakeThermalBombStairs(class ABBRoomBase* Room, bool StairsGoUp);
	static void UseVirus(class ABBPlayerCharacterBase* Character);
	static TArray<class ABBRoomBase*> GetValidVirusRooms(class ABBPlayerCharacterBase* Character);
	static void OnVirusRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom);

	static void EndChooseTool(UObject* WorldContext);
};
