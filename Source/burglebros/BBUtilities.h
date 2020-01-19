// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BBRoomPosition.h"
#include "DateTime.h"
#include "BBGameInstance.h"
#include "BBPlayerClass.h"
#include "BBUtilities.generated.h"


USTRUCT(BlueprintType)
struct FDiceThrowResult
{
	GENERATED_BODY();
public:
	FDiceThrowResult()
	{
	}
	FDiceThrowResult(int32 SingleResult)
	{
		Results.Add(SingleResult);
	}
	FDiceThrowResult(TArray<int32> MultipleResults)
	{
		Results.Append(MultipleResults);
	}
	int32 GetFirstResult()
	{
		if (Results.Num() > 0)
			return Results[0];
		return -1;
	}
	TArray<int32> GetAllResults()
	{
		return TArray<int32>(Results);
	}
	bool ContainsNumber(int32 Number)
	{
		return Results.Contains(Number);
	}
private:
	TArray<int32> Results;
};

UCLASS()
class BURGLEBROS_API UBBUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static EDirection GetOppositeDirection(EDirection Direction);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static class ABBMainGameMode* GetMainGameMode(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static class ABBPlayerController* GetBBPlayerController(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static class ABBLevel* GetCurrentLevel(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
		static FDiceThrowResult ThrowDice(int32 AmountDice);

	UFUNCTION(BlueprintCallable)
		static TArray<class ABBRoomBase*> GetRoomsAround(class ABBRoomBase* MiddleRoom);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static class ABBPlayerCameraManager* GetPlayerCameraManager(UObject* WorldContextObject, int32 PlayerIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static class UBBGameInstance* GetBBGameInstance(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
		static EPlayerClass GetActualPlayerClass(EPlayerClass PlayerClass, bool IsAlternative);

	// Returns -1 for non computer room types
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static int32 GetComputerRoomTokenAmount(UObject* WorldContextObject, ERoomType ComputerType);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static bool HackerTokenPresent(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static bool IsInEditor(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
		static void ChangeVolume(class USoundClass* SoundClass, float NewVolume);

	template<typename T>
	static FORCEINLINE void ShuffleArray(TArray<T>& Array);

	static FRandomStream RandomStream;
	static void InitRandomStream();
};

template<typename T>
void UBBUtilities::ShuffleArray(TArray<T>& Array)
{
	for (int32 Index = Array.Num() - 1; Index > 0; Index--)
	{
		int32 RandomIndex = RandomStream.RandRange(0, Index);
		Array.Swap(Index, RandomIndex);
	}
}