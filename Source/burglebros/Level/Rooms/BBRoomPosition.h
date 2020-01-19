// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBRoomPosition.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	North, East, South, West
};

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Safe,
	Stairs,
	Walkway,
	Laboratory,
	Lavatory,
	ServiceDuct,
	SecretDoor,
	ComputerLaser,
	ComputerMotion,
	ComputerFingerprint,
	Camera,
	Laser,
	Motion,
	Detector,
	Fingerprint,
	Thermo,
	Keypad,
	Deadbolt,
	Foyer,
	Atrium
};

USTRUCT(BlueprintType)
struct FRoomPosition
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int32 PosX = 0;
	UPROPERTY(BlueprintReadWrite)
		int32 PosY = 0;
	UPROPERTY(BlueprintReadWrite)
		int32 Floor = 0;

	FRoomPosition(int32 PosX, int32 PosY, int32 Floor)
		: PosX(PosX), PosY(PosY), Floor(Floor)
	{
	}
	FRoomPosition()
	{
	}

	bool IsSameRoom(FRoomPosition& Other)
	{
		return PosX == Other.PosX
			&& PosY == Other.PosY
			&& Floor == Other.Floor;
	}

	bool IsNextToRoom(FRoomPosition& Other)
	{
		if (Other.PosX == PosX)
		{
			return FMath::Abs(Other.PosY - PosY) == 1;
		}
		if (Other.PosY == PosY)
		{
			return FMath::Abs(Other.PosX - PosX) == 1;
		}
		return false;
	}

	FRoomPosition GetAboveRoomPosition()
	{
		return FRoomPosition(PosX, PosY, Floor + 1);
	}
	FRoomPosition GetBelowRoomPosition()
	{
		return FRoomPosition(PosX, PosY, Floor - 1);
	}
};
