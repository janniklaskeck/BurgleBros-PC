// Fill out your copyright notice in the Description page of Project Settings.

#include "BBLevel.h"
#include "BBMainGameMode.h"
#include "BBUtilities.h"
#include "BBPlayerCharacterBase.h"

ABBLevel::ABBLevel()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABBLevel::BeginPlay()
{
	Super::BeginPlay();
	UBBUtilities::GetMainGameMode(this)->SetCurrentLevel(this);
}

TArray<TSubclassOf<ABBRoomBase>> ABBLevel::GetFloorDeck()
{
	TArray<TSubclassOf<ABBRoomBase>> FloorDeck;

	FloorDeck.Append(GuaranteedRoomClasses);
	int32 AdditionalRoomsToAdd = FMath::Max(0, 14 - FloorDeck.Num());
	for (int32 Index = 0; Index < AdditionalRoomsToAdd; Index++)
	{
		TSubclassOf<ABBRoomBase> RandomRoomClass = AllRoomClasses[AvailableRoomClasses.Pop()].RoomClass;
		FloorDeck.Add(RandomRoomClass);
	}
	if (FloorDeck.Num() == 14)
	{
		FloorDeck.Add(AllRoomClasses[ERoomType::Safe].RoomClass);
		FloorDeck.Add(AllRoomClasses[ERoomType::Stairs].RoomClass);
	}
	UBBUtilities::ShuffleArray<TSubclassOf<ABBRoomBase>>(FloorDeck);
	return FloorDeck;
}

void ABBLevel::BuildLevel()
{
	BuildFloors();
}

FFloor& ABBLevel::GetFloor(int32 Index)
{
	return Floors[Index];
}

ABBRoomBase* ABBLevel::GetRoomAt(FRoomPosition RoomPosition)
{
	int32 FloorIndex = RoomPosition.Floor;
	if (FloorIndex < 0 || FloorIndex >= AmountFloors)
	{
		return nullptr;
	}
	TArray<ABBRoomBase*> Rooms = Floors[FloorIndex].Rooms;
	for (ABBRoomBase* Room : Rooms)
	{
		if (Room->Position.IsSameRoom(RoomPosition))
		{
			return Room;
		}
	}
	return nullptr;
}

ABBRoomBase* ABBLevel::GetRoomAt(int32 PosX, int32 PosY, int32 Floor)
{
	FRoomPosition Position = FRoomPosition(PosX, PosY, Floor);
	return GetRoomAt(Position);
}

TArray<ABBRoomBase*> ABBLevel::GetRoomsOfFloor(int32 Floor)
{
	TArray<ABBRoomBase*> Rooms;
	for (ABBRoomBase* Room : Floors[Floor].Rooms)
	{
		Rooms.Add(Room);
	}
	return Rooms;
}

TArray<ABBRoomBase*> ABBLevel::GetRoomsOfFloor(class ABBCharacterBase* Character)
{
	return GetRoomsOfFloor(Character->CurrentRoom->Position.Floor);
}

TArray<ABBRoomBase*> ABBLevel::GetRoomsOfType(ERoomType RoomType)
{
	TArray<ABBRoomBase*> RoomsOfType;
	for (FFloor Floor : Floors)
	{
		for (ABBRoomBase* Room : Floor.Rooms)
		{
			if (Room->RoomType == RoomType)
			{
				RoomsOfType.Add(Room);
			}
		}
	}
	return RoomsOfType;
}

void ABBLevel::BuildFloors()
{
	Floors.Reset(3);
	for (const TPair<ERoomType, FRoomClass>& RoomPair : AllRoomClasses)
	{
		if (RoomPair.Key != ERoomType::Safe && RoomPair.Key != ERoomType::Stairs)
		{
			for (int32 Index = 0; Index < RoomPair.Value.AmountAvailable; Index++)
			{
				AvailableRoomClasses.Add(RoomPair.Key);
			}
		}
	}
	UBBUtilities::ShuffleArray<ERoomType>(AvailableRoomClasses);
	for (int32 Index = 0; Index < AmountFloors; Index++)
	{
		Floors.Add(FFloor(Index));
		if (Index == 0)
		{
			Floors[0].RevealFloor();
		}
		else
		{
			Floors[Index].OnFloorRevealed.AddDynamic(this, &ABBLevel::SpawnGuardInRevealedFloor);
		}
		PlaceTiles(Index);
		PlaceWalls(Index);
		if (Index != 0)
		{
			Floors[Index].SetFloorHidden(true);
		}
	}
	ABBRoomBase::AmountAlarms.Reset(AmountFloors);
	for (int32 Index = 0; Index < AmountFloors; Index++)
	{
		ABBRoomBase::AmountAlarms.Add(0);
	}
	TArray<ABBRoomBase*> StairRooms = GetRoomsOfType(ERoomType::Stairs);
	for (ABBRoomBase* Room : StairRooms)
	{
		ABBRoomBase* RoomAbove = GetRoomAt(Room->Position.GetAboveRoomPosition());
		if (RoomAbove)
		{
			RoomAbove->SetHasStairsDown(true);
		}
	}
}

void ABBLevel::PlaceTiles(int32 FloorIndex)
{
	UWorld* World = GetWorld();
	if (!World)
		return;
	TArray<TSubclassOf<ABBRoomBase>> FloorDeck = GetFloorDeck();
	for (int32 X = 0; X < FloorWidth; X++)
	{
		for (int32 Y = 0; Y < FloorWidth; Y++)
		{
			FVector Position = FVector(X * TileWidth, Y * TileWidth, FloorIndex * FloorHeight);
			FRotator Rotation = FRotator::ZeroRotator;
			ABBRoomBase* SpawnedRoom = World->SpawnActor<ABBRoomBase>(FloorDeck.Pop(), Position, Rotation);
			SpawnedRoom->CurrentLevel = this;
			SpawnedRoom->SetPosition(FRoomPosition(X, Y, FloorIndex));
			Floors[FloorIndex].Rooms.Add(SpawnedRoom);
		}
	}
}

void ABBLevel::PlaceWalls(int32 FloorIndex)
{
	// TODO Random placement
	for (ABBRoomBase* Room : Floors[FloorIndex].Rooms)
	{
		if (Room->Position.PosX == 0)
		{
			Room->SetWall(EDirection::West, true);
		}
		if (Room->Position.PosY == 0)
		{
			Room->SetWall(EDirection::South, true);
		}
		if (Room->Position.PosX == FloorWidth - 1)
		{
			Room->SetWall(EDirection::East, true);
		}
		if (Room->Position.PosY == FloorWidth - 1)
		{
			Room->SetWall(EDirection::North, true);
		}
		Room->UpdateWalls();
	}
	if (FloorIndex == 0)
	{
		PlaceWallInRoom(GetRoomAt(0, 0, 0), EDirection::East);
		PlaceWallInRoom(GetRoomAt(2, 0, 0), EDirection::North);
		PlaceWallInRoom(GetRoomAt(0, 1, 0), EDirection::North);
		PlaceWallInRoom(GetRoomAt(1, 1, 0), EDirection::North);
		PlaceWallInRoom(GetRoomAt(2, 1, 0), EDirection::East);
		PlaceWallInRoom(GetRoomAt(3, 2, 0), EDirection::North);
		PlaceWallInRoom(GetRoomAt(0, 3, 0), EDirection::East);
		PlaceWallInRoom(GetRoomAt(1, 3, 0), EDirection::East);
	}
	else if (FloorIndex == 1)
	{
		PlaceWallInRoom(GetRoomAt(0, 0, 1), EDirection::East);
		PlaceWallInRoom(GetRoomAt(1, 0, 1), EDirection::East);
		PlaceWallInRoom(GetRoomAt(2, 0, 1), EDirection::East);
		PlaceWallInRoom(GetRoomAt(1, 1, 1), EDirection::North);
		PlaceWallInRoom(GetRoomAt(2, 1, 1), EDirection::North);
		PlaceWallInRoom(GetRoomAt(0, 3, 1), EDirection::East);
		PlaceWallInRoom(GetRoomAt(1, 3, 1), EDirection::East);
		PlaceWallInRoom(GetRoomAt(2, 3, 1), EDirection::East);
	}
	else if (FloorIndex == 2)
	{
		PlaceWallInRoom(GetRoomAt(2, 0, 2), EDirection::North);
		PlaceWallInRoom(GetRoomAt(0, 1, 2), EDirection::East);
		PlaceWallInRoom(GetRoomAt(0, 1, 2), EDirection::North);
		PlaceWallInRoom(GetRoomAt(2, 1, 2), EDirection::East);
		PlaceWallInRoom(GetRoomAt(2, 1, 2), EDirection::North);
		PlaceWallInRoom(GetRoomAt(0, 2, 2), EDirection::East);
		PlaceWallInRoom(GetRoomAt(1, 2, 2), EDirection::East);
		PlaceWallInRoom(GetRoomAt(2, 3, 2), EDirection::East);
	}
}

bool ABBLevel::CanPlaceWallAt(ABBRoomBase* Room, EDirection Side)
{
	int32 ExistingWalls = 0;
	if (Room->Walls[EDirection::North])
		++ExistingWalls;
	if (Room->Walls[EDirection::East])
		++ExistingWalls;
	if (Room->Walls[EDirection::South])
		++ExistingWalls;
	if (Room->Walls[EDirection::West])
		++ExistingWalls;
	if (ExistingWalls > 2)
		return false;

	return !Room->Walls[Side];
}

void ABBLevel::PlaceWallInRoom(ABBRoomBase* Room, EDirection Side)
{
	ensure(Room);
	if (CanPlaceWallAt(Room, Side))
	{
		Room->SetWall(Side, true);
		Room->UpdateWalls();
		ABBRoomBase* Neighbour = GetNeighbourRoomInDirection(Room, Side);
		if (Neighbour)
		{
			EDirection Opposite = UBBUtilities::GetOppositeDirection(Side);
			PlaceWallInRoom(Neighbour, Opposite);
		}
	}
}

void ABBLevel::SpawnGuardInRevealedFloor(int32 Floor)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	GameMode->SpawnGuard(Floor, 2 + Floor, true);
}

void ABBLevel::DiscoverAllRooms()
{
	for (FFloor Floor : Floors)
	{
		for (ABBRoomBase* Room : Floor.Rooms)
		{
			Room->SetDiscovered();
		}
	}
}

ABBRoomBase* ABBLevel::GetNeighbourRoomInDirection(ABBRoomBase* OriginalRoom, EDirection Direction)
{
	int32 PosX = OriginalRoom->Position.PosX;
	int32 PosY = OriginalRoom->Position.PosY;
	switch (Direction)
	{
	case EDirection::North:
		++PosY;
		break;
	case EDirection::East:
		++PosX;
		break;
	case EDirection::South:
		--PosY;
		break;
	case EDirection::West:
		--PosX;
		break;
	}
	return GetRoomAt(PosX, PosY, OriginalRoom->Position.Floor);
}

TArray<class ABBPlayerCharacterBase*> ABBLevel::GetPlayersOnFloor(int32 FloorIndex)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	TArray<ABBPlayerCharacterBase*> PlayersOnFloor;
	for (ABBCharacterBase* Player : GameMode->GetAllPlayers())
	{
		if (Player->CurrentRoom->Position.Floor == FloorIndex)
		{
			PlayersOnFloor.Add(Cast<ABBPlayerCharacterBase>(Player));
		}
	}
	return PlayersOnFloor;
}

FRoomPosition ABBLevel::GetRandomRoomPositionOnFloor(int32 Floor)
{
	int32 X = FMath::RandRange(0, FloorWidth - 1);
	int32 Y = FMath::RandRange(0, FloorWidth - 1);
	return FRoomPosition(X, Y, Floor);
}

