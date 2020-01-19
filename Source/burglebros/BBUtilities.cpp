// Fill out your copyright notice in the Description page of Project Settings.

#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BBPlayerController.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBPlayerCameraManager.h"
#include "BBComputerComponent.h"
#include "BBHackerComponent.h"
#include "Sound/SoundClass.h"

FRandomStream UBBUtilities::RandomStream;

void UBBUtilities::InitRandomStream()
{
	RandomStream = FRandomStream();
	int64 NewSeed = FDateTime::Now().ToUnixTimestamp();
	RandomStream.Initialize(NewSeed);
	UE_LOG(LogTemp, Warning, TEXT("Seed: %d"), NewSeed);
}

EDirection UBBUtilities::GetOppositeDirection(EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::North:
		return EDirection::South;
		break;
	case EDirection::East:
		return EDirection::West;
		break;
	case EDirection::South:
		return EDirection::North;
		break;
	case EDirection::West:
		return EDirection::East;
		break;
	default:
		return EDirection::North;
	}
}

ABBMainGameMode* UBBUtilities::GetMainGameMode(UObject* WorldContext)
{
	return Cast<ABBMainGameMode>(UGameplayStatics::GetGameMode(WorldContext));
}

ABBPlayerController* UBBUtilities::GetBBPlayerController(UObject* WorldContextObject)
{
	return Cast<ABBPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
}

class ABBLevel* UBBUtilities::GetCurrentLevel(UObject* WorldContextObject)
{
	return GetMainGameMode(WorldContextObject)->GetCurrentLevel();
}

FDiceThrowResult UBBUtilities::ThrowDice(int32 AmountDice)
{
	if (AmountDice < 1)
		return FDiceThrowResult();
	if (AmountDice == 1)
	{
		return FDiceThrowResult(RandomStream.RandRange(1, 6));
	}
	TArray<int32> Results;
	for (int32 Index = 0; Index < AmountDice; Index++)
	{
		Results.Add(RandomStream.RandRange(1, 6));
	}
	return FDiceThrowResult(Results);
}

TArray<class ABBRoomBase*> UBBUtilities::GetRoomsAround(class ABBRoomBase* MiddleRoom)
{
	FRoomPosition CurrentPosition = MiddleRoom->Position;
	ABBLevel* CurrentLevel = MiddleRoom->CurrentLevel;
	TArray<ABBRoomBase*> ValidNeighbourRooms;
	if (!MiddleRoom->Walls[EDirection::North])
	{
		ValidNeighbourRooms.Add(CurrentLevel->GetNeighbourRoomInDirection(MiddleRoom, EDirection::North));
	}
	if (!MiddleRoom->Walls[EDirection::East])
	{
		ValidNeighbourRooms.Add(CurrentLevel->GetNeighbourRoomInDirection(MiddleRoom, EDirection::East));
	}
	if (!MiddleRoom->Walls[EDirection::South])
	{
		ValidNeighbourRooms.Add(CurrentLevel->GetNeighbourRoomInDirection(MiddleRoom, EDirection::South));
	}
	if (!MiddleRoom->Walls[EDirection::West])
	{
		ValidNeighbourRooms.Add(CurrentLevel->GetNeighbourRoomInDirection(MiddleRoom, EDirection::West));
	}
	return ValidNeighbourRooms;
}

ABBPlayerCameraManager* UBBUtilities::GetPlayerCameraManager(UObject* WorldContextObject, int32 PlayerIndex)
{
	return Cast<ABBPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(WorldContextObject, PlayerIndex));
}

class UBBGameInstance* UBBUtilities::GetBBGameInstance(UObject* WorldContextObject)
{
	return Cast<UBBGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

EPlayerClass UBBUtilities::GetActualPlayerClass(EPlayerClass PlayerClass, bool IsAlternative)
{
	if (IsAlternative)
	{
		switch (PlayerClass)
		{
		case EPlayerClass::Acrobat:
			return EPlayerClass::Acrobat_Alt;
		case EPlayerClass::Hacker:
			return EPlayerClass::Hacker_Alt;
		case EPlayerClass::Hawk:
			return EPlayerClass::Hawk_Alt;
		case EPlayerClass::Juicer:
			return EPlayerClass::Juicer_Alt;
		case EPlayerClass::Peterman:
			return EPlayerClass::Peterman_Alt;
		case EPlayerClass::Raven:
			return EPlayerClass::Raven_Alt;
		case EPlayerClass::Rigger:
			return EPlayerClass::Rigger_Alt;
		case EPlayerClass::Rook:
			return EPlayerClass::Rook_Alt;
		case EPlayerClass::Spotter:
			return EPlayerClass::Spotter_Alt;
		}
	}
	return PlayerClass;
}

int32 UBBUtilities::GetComputerRoomTokenAmount(UObject* WorldContextObject, ERoomType ComputerType)
{
	if (ComputerType != ERoomType::ComputerFingerprint && ComputerType != ERoomType::ComputerLaser && ComputerType != ERoomType::ComputerMotion)
		return -1;
	int32 AmountTokens = 0;
	ABBMainGameMode* GameMode = GetMainGameMode(WorldContextObject);
	TArray<ABBRoomBase*> ComputerRooms = GameMode->GetCurrentLevel()->GetRoomsOfType(ComputerType);
	for (ABBRoomBase* Room : ComputerRooms)
	{
		AmountTokens += Room->GetRoomFunctionAs<UBBComputerComponent>()->AmountTokens;
	}
	return AmountTokens;
}

bool UBBUtilities::HackerTokenPresent(UObject* WorldContextObject)
{
	ABBMainGameMode* GameMode = GetMainGameMode(WorldContextObject);
	ABBPlayerCharacterBase* Hacker = GameMode->GetPlayerWithType(EPlayerClass::Hacker_Alt);
	if (Hacker)
	{
		return Cast<UBBHackerComponent>(Hacker->GetClassComponent())->HasToken();
	}
	return false;
}

bool UBBUtilities::IsInEditor(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	ensure(World);
	return World->WorldType != EWorldType::Game;
}

void UBBUtilities::ChangeVolume(class USoundClass* SoundClass, float NewVolume)
{
	if (SoundClass)
	{
		SoundClass->Properties.Volume = FMath::Clamp(NewVolume, 0.f, 1.f);
	}
}
