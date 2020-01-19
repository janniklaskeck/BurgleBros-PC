// Fill out your copyright notice in the Description page of Project Settings.

#include "BBHawkComponent.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "BBPlayerCharacterBase.h"

bool UBBHawkComponent::CanUseAbility()
{
	if (PlayerClass == EPlayerClass::Hawk)
	{
		return !bUsedAbility;
	}
	else
	{
		return !bUsedAbility;
	}
}

bool UBBHawkComponent::IsActiveAbility()
{
	return true;
}

void UBBHawkComponent::UseAbility()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->OnRoomChosen.AddDynamic(this, &UBBHawkComponent::OnValidRoomChosen);
	if (PlayerClass == EPlayerClass::Hawk)
	{
		GameMode->SetValidRoomsToChoose(GetValidThroughWallRooms());
	}
	else
	{
		GameMode->SetValidRoomsToChoose(GetValidTwoDistanceRooms());
	}
	GameMode->EnterChooseRoom();
}

TArray<class ABBRoomBase*> UBBHawkComponent::GetValidThroughWallRooms()
{
	ABBLevel* CurrentLevel = GetOwnerCharacter()->CurrentLevel;
	ABBRoomBase* CurrentRoom = GetOwnerCharacter()->CurrentRoom;
	FRoomPosition CurrentRoomPosition = CurrentRoom->Position;
	TArray<ABBRoomBase*> ValidRooms;

	FRoomPosition NorthPosition(CurrentRoomPosition.PosX, CurrentRoomPosition.PosY + 1, CurrentRoomPosition.Floor);
	ABBRoomBase* NorthRoom = CurrentLevel->GetRoomAt(NorthPosition);
	if (NorthRoom && CurrentRoom->Walls[EDirection::North] && !NorthRoom->bIsDiscovered)
	{
		ValidRooms.Add(NorthRoom);
	}

	FRoomPosition EastPosition(CurrentRoomPosition.PosX + 1, CurrentRoomPosition.PosY, CurrentRoomPosition.Floor);
	ABBRoomBase* EastRoom = CurrentLevel->GetRoomAt(EastPosition);
	if (EastRoom && CurrentRoom->Walls[EDirection::East] && !EastRoom->bIsDiscovered)
	{
		ValidRooms.Add(EastRoom);
	}

	FRoomPosition SouthPosition(CurrentRoomPosition.PosX, CurrentRoomPosition.PosY - 1, CurrentRoomPosition.Floor);
	ABBRoomBase* SouthRoom = CurrentLevel->GetRoomAt(SouthPosition);
	if (SouthRoom && CurrentRoom->Walls[EDirection::South] && !SouthRoom->bIsDiscovered)
	{
		ValidRooms.Add(SouthRoom);
	}

	FRoomPosition WestPosition(CurrentRoomPosition.PosX - 1, CurrentRoomPosition.PosY, CurrentRoomPosition.Floor);
	ABBRoomBase* WestRoom = CurrentLevel->GetRoomAt(WestPosition);
	if (WestRoom && CurrentRoom->Walls[EDirection::West] && !WestRoom->bIsDiscovered)
	{
		ValidRooms.Add(WestRoom);
	}
	return ValidRooms;
}

TArray<class ABBRoomBase*> UBBHawkComponent::GetValidTwoDistanceRooms()
{
	ABBPlayerCharacterBase* OwnerCharacter = GetOwnerCharacter();
	TArray<ABBRoomBase*> RoomsOfFloor = OwnerCharacter->CurrentLevel->GetRoomsOfFloor(OwnerCharacter);
	TArray<ABBRoomBase*> ValidRooms;
	for (ABBRoomBase* Room : RoomsOfFloor)
	{
		FRoomPosition CurrentPosition = OwnerCharacter->CurrentRoom->Position;
		int32 XDistance = FMath::Abs(Room->Position.PosX - CurrentPosition.PosX);
		int32 YDistance = FMath::Abs(Room->Position.PosY - CurrentPosition.PosY);
		bool XPosInRange = XDistance <= 2;
		bool YPosInRange = YDistance <= 2;
		if (XPosInRange && YPosInRange)
		{
			TArray<FRoomPosition> Path = OwnerCharacter->GetPath(Room->Position);
			if (Path.Num() > 0 && Path.Num() < 3)
			{
				if (Path.Num() == 1)
				{
					ValidRooms.Add(Room);
				}
				else if (Path.Num() == 2 && OwnerCharacter->CurrentLevel->GetRoomAt(Path[0])->bIsDiscovered)
				{
					ValidRooms.Add(Room);
				}
				ABBRoomBase* FirstRoom = OwnerCharacter->CurrentLevel->GetRoomAt(Path[0]);
				if (FirstRoom->bIsDiscovered && FirstRoom->RoomType == ERoomType::Stairs)
				{
					if (Path[0].Floor > 0)
					{
						ValidRooms.Add(OwnerCharacter->CurrentLevel->GetRoomAt(Path[0].GetBelowRoomPosition()));
					}
					if (Path[0].Floor < OwnerCharacter->CurrentLevel->AmountFloors - 1)
					{
						ValidRooms.Add(OwnerCharacter->CurrentLevel->GetRoomAt(Path[0].GetAboveRoomPosition()));
					}
				}
			}
		}
	}
	return ValidRooms;
}

void UBBHawkComponent::OnValidRoomChosen(class ABBRoomBase* ChosenRoom)
{
	if (ChosenRoom)
	{
		bUsedAbility = true;
		ChosenRoom->SetDiscovered();
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->OnRoomChosen.RemoveDynamic(this, &UBBHawkComponent::OnValidRoomChosen);
}
