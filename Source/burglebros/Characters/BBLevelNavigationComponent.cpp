// Fill out your copyright notice in the Description page of Project Settings.

#include "BBLevelNavigationComponent.h"
#include "BBGuard.h"
#include "BBLevel.h"
#include "BBAIController.h"
#include "BBLevel.h"
#include "BBMainGameMode.h"
#include "BBUtilities.h"
#include "Algo/Reverse.h"

DEFINE_LOG_CATEGORY(LogLevelNavigation);

UBBLevelNavigationComponent::UBBLevelNavigationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UBBLevelNavigationComponent::CanMoveToRoom(ABBRoomBase* Room)
{
	ABBCharacterBase* ControlledPawn = GetOwnerCharacter();
	if (!ControlledPawn || !Room || ControlledPawn->CurrentRoom == Room)
		return false;
	TArray<FRoomPosition> PathToRoom = GetPath(ControlledPawn->CurrentRoom, Room);
	if (PathToRoom.Num() > 0)
	{
		ABBRoomBase* FirstRoomOnPath = GetOwnerCharacter()->CurrentLevel->GetRoomAt(PathToRoom[0]);
		bool HasEnoughActions = PathToRoom.Num() <= ControlledPawn->Actions.GetCurrentActions();
		return HasEnoughActions && FirstRoomOnPath->CanEnterRoom(ControlledPawn);
	}
	return false;
}

bool UBBLevelNavigationComponent::CanMoveToRoomPosition(const FRoomPosition& RoomPosition)
{
	FRoomPosition RoomPositionCopy(RoomPosition);
	ABBRoomBase* Room = GetCurrentLevel()->GetRoomAt(RoomPositionCopy);
	if (!Room)
		return false;
	return CanMoveToRoom(Room);
}

void UBBLevelNavigationComponent::MoveToRoom(ABBRoomBase* Room, bool IgnoreEffects /*= false*/, bool IgnoreCost /*= false*/)
{
	if (!Room)
		return;
	MoveToRoomPosition(Room->Position, IgnoreEffects, IgnoreCost);
}

void UBBLevelNavigationComponent::MoveToRoomPosition(const FRoomPosition& RoomPosition, bool IgnoreEffects /*= false*/, bool IgnoreCost /* = false */)
{
	IgnoreCostOnNextRoom = IgnoreCost;
	IgnoreEffectsOnNextRoom = IgnoreEffects;
	ABBCharacterBase* CharacterPawn = GetOwnerCharacter();
	CurrentMovementPath.Reset(6);
	FRoomPosition RoomPositionCopy(RoomPosition);
	CurrentMovementPath.Append(GetPath(CharacterPawn->CurrentRoom->Position, RoomPositionCopy));

	ABBAIController* CharacterAIController = GetOwnerAIController();
	FVector TargetLocation = CharacterPawn->CurrentLevel->GetRoomAt(CurrentMovementPath[0])->GetFreePlayerPosition();
	CharacterAIController->OnMoveSuccess.Clear(); // TODO look for better alternative
	CharacterAIController->OnMoveSuccess.AddDynamic(this, &UBBLevelNavigationComponent::OnRoomReached);
	CharacterAIController->MoveToTarget(TargetLocation, AcceptanceRadius);
	bIsMoving = true;
}

bool UBBLevelNavigationComponent::IsMoving()
{
	return bIsMoving;
}

void UBBLevelNavigationComponent::OnRoomReached()
{
	FRoomPosition EnteredRoomPosition = CurrentMovementPath[0];
	ABBCharacterBase* ControlledPawn = GetOwnerCharacter();
	ABBRoomBase* EnteredRoom = ControlledPawn->CurrentLevel->GetRoomAt(EnteredRoomPosition);
	CurrentMovementPath.RemoveAt(0);
	bool CanMoveFurther = true;
	if (EnteredRoom->CanEnterRoom(ControlledPawn) || IgnoreCostOnNextRoom)
	{
		ControlledPawn->CurrentRoom->LeaveRoom(ControlledPawn);
		CanMoveFurther = EnteredRoom->EnterRoom(ControlledPawn, IgnoreEffectsOnNextRoom, IgnoreCostOnNextRoom);
		IgnoreCostOnNextRoom = false;
		IgnoreEffectsOnNextRoom = false;
		if (OnRoomEntered.IsBound())
			OnRoomEntered.Broadcast(EnteredRoom);
	}
	else
	{ // TODO move back to previous Room
		if (OnRoomBlocked.IsBound())
			OnRoomBlocked.Broadcast(EnteredRoom);
	}
	ABBAIController* CharacterAIController = GetOwnerAIController();
	if (CanMoveFurther && CurrentMovementPath.Num() > 0 && CanMoveToRoomPosition(CurrentMovementPath[0]))
	{
		ABBRoomBase* NextRoom = ControlledPawn->CurrentLevel->GetRoomAt(CurrentMovementPath[0]);
		FVector TargetLocation;
		if (IsGuard())
		{
			TargetLocation = NextRoom->GetGuardPosition();
		}
		else
		{
			TargetLocation = NextRoom->GetFreePlayerPosition();
		}
		CharacterAIController->MoveToTarget(TargetLocation, AcceptanceRadius);
	}
	else
	{
		CharacterAIController->OnMoveSuccess.Clear();
		if (OnFinalRoomEntered.IsBound())
			OnFinalRoomEntered.Broadcast(EnteredRoom);
		bIsMoving = false;
	}
}

TArray<FRoomPosition> UBBLevelNavigationComponent::GetPath(class ABBRoomBase* Start, class ABBRoomBase* End)
{
	return GetPath(Start->Position, End->Position);
}

TArray<FRoomPosition> UBBLevelNavigationComponent::GetPath(FRoomPosition& Start, FRoomPosition& Target)
{
	if (!IsValidRoom(Start.PosX, Start.PosY) || !IsValidRoom(Target.PosX, Target.PosY) || Start.Floor != Target.Floor)
	{
		UE_LOG(LogLevelNavigation, Warning, TEXT("Invalid Start or Target for A* Search"));
		return TArray<FRoomPosition>();
	}
	if (Start.IsSameRoom(Target))
	{
		UE_LOG(LogLevelNavigation, Warning, TEXT("Already at Target"));
		return TArray<FRoomPosition>();
	}
	TArray<TArray<bool>> ClosedList;
	TArray<TArray<FCell>> Cells;

	int32 FloorWidth = GetCurrentLevel()->FloorWidth;
	for (int32 X = 0; X < FloorWidth; X++)
	{
		ClosedList.Add(TArray<bool>());
		Cells.Add(TArray<FCell>());
		for (int32 Y = 0; Y < FloorWidth; Y++)
		{
			ClosedList[X].Add(false);
			Cells[X].Add(FCell());
		}
	}
	Cells[Start.PosX][Start.PosY].F = 0.f;
	Cells[Start.PosX][Start.PosY].G = 0.f;
	Cells[Start.PosX][Start.PosY].H = 0.f;
	Cells[Start.PosX][Start.PosY].Parent = FRoomPosition(Start.PosX, Start.PosY, Start.Floor);

	TArray<PPair> OpenList;
	OpenList.Add(PPair(0.f, Start));

	bool FoundTarget = false;

	while (OpenList.Num() > 0)
	{
		PPair FirstPair = OpenList[0];
		OpenList.RemoveAt(0);
		FRoomPosition FirstPosition = FirstPair.Second;
		ClosedList[FirstPosition.PosX][FirstPosition.PosY] = true;

		float GNew, HNew, FNew;

		/*
		North
		East
		South
		West
		*/
		// North
		{
			FRoomPosition NorthRoom = FRoomPosition(FirstPosition.PosX, FirstPosition.PosY + 1, FirstPosition.Floor);
			if (IsValidRoom(NorthRoom.PosX, NorthRoom.PosY))
			{
				if (Target.IsSameRoom(NorthRoom) && !IsDirectionBlocked(FirstPosition, EDirection::North))
				{
					Cells[NorthRoom.PosX][NorthRoom.PosY].Parent = FirstPosition;
					//UE_LOG(LogLevelNavigation, Warning, TEXT("Target Found!"));
					FoundTarget = true;
					return ConvertPath(Cells, Target);
				}
				else if (!ClosedList[NorthRoom.PosX][NorthRoom.PosY] && !IsDirectionBlocked(FirstPosition, EDirection::North))
				{
					GNew = Cells[FirstPosition.PosX][FirstPosition.PosY].G + 1.f;
					HNew = CalculateHeuristic(NorthRoom, Target);
					FNew = GNew + HNew;

					FCell& NorthCell = Cells[NorthRoom.PosX][NorthRoom.PosY];
					if (NorthCell.IsFMax() || NorthCell.F > FNew)
					{
						PPair NorthRoomPair = PPair(FNew, NorthRoom);
						if (!OpenList.Contains(NorthRoomPair))
						{
							OpenList.Add(NorthRoomPair);
						}
						NorthCell.F = FNew;
						NorthCell.G = GNew;
						NorthCell.H = HNew;
						NorthCell.Parent = FirstPosition;
					}
				}
			}
		}
		// East
		{
			FRoomPosition EastRoom = FRoomPosition(FirstPosition.PosX + 1, FirstPosition.PosY, FirstPosition.Floor);
			if (IsValidRoom(EastRoom.PosX, EastRoom.PosY))
			{
				if (Target.IsSameRoom(EastRoom) && !IsDirectionBlocked(FirstPosition, EDirection::East))
				{
					Cells[EastRoom.PosX][EastRoom.PosY].Parent = FirstPosition;
					//UE_LOG(LogLevelNavigation, Warning, TEXT("Target Found!"));
					FoundTarget = true;
					return ConvertPath(Cells, Target);
				}
				else if (!ClosedList[EastRoom.PosX][EastRoom.PosY] && !IsDirectionBlocked(FirstPosition, EDirection::East))
				{
					GNew = Cells[FirstPosition.PosX][FirstPosition.PosY].G + 1.f;
					HNew = CalculateHeuristic(EastRoom, Target);
					FNew = GNew + HNew;

					FCell& EastCell = Cells[EastRoom.PosX][EastRoom.PosY];
					if (EastCell.IsFMax() || EastCell.F > FNew)
					{
						PPair EastRoomPair = PPair(FNew, EastRoom);
						if (!OpenList.Contains(EastRoomPair))
						{
							OpenList.Add(EastRoomPair);
						}
						EastCell.F = FNew;
						EastCell.G = GNew;
						EastCell.H = HNew;
						EastCell.Parent = FirstPosition;
					}
				}
			}
		}
		// South
		{
			FRoomPosition SouthRoom = FRoomPosition(FirstPosition.PosX, FirstPosition.PosY - 1, FirstPosition.Floor);
			if (IsValidRoom(SouthRoom.PosX, SouthRoom.PosY))
			{
				if (Target.IsSameRoom(SouthRoom) && !IsDirectionBlocked(FirstPosition, EDirection::South))
				{
					Cells[SouthRoom.PosX][SouthRoom.PosY].Parent = FirstPosition;
					//UE_LOG(LogLevelNavigation, Warning, TEXT("Target Found!"));
					FoundTarget = true;
					return ConvertPath(Cells, Target);
				}
				else if (!ClosedList[SouthRoom.PosX][SouthRoom.PosY] && !IsDirectionBlocked(FirstPosition, EDirection::South))
				{
					GNew = Cells[FirstPosition.PosX][FirstPosition.PosY].G + 1.f;
					HNew = CalculateHeuristic(SouthRoom, Target);
					FNew = GNew + HNew;

					FCell& SouthCell = Cells[SouthRoom.PosX][SouthRoom.PosY];
					if (SouthCell.IsFMax() || SouthCell.F > FNew)
					{
						PPair SouthRoomPair = PPair(FNew, SouthRoom);
						if (!OpenList.Contains(SouthRoomPair))
						{
							OpenList.Add(SouthRoomPair);
						}
						SouthCell.F = FNew;
						SouthCell.G = GNew;
						SouthCell.H = HNew;
						SouthCell.Parent = FirstPosition;
					}
				}
			}
		}
		// West
		{
			FRoomPosition WestRoom = FRoomPosition(FirstPosition.PosX - 1, FirstPosition.PosY, FirstPosition.Floor);
			if (IsValidRoom(WestRoom.PosX, WestRoom.PosY))
			{
				if (Target.IsSameRoom(WestRoom) && !IsDirectionBlocked(FirstPosition, EDirection::West))
				{
					Cells[WestRoom.PosX][WestRoom.PosY].Parent = FirstPosition;
					//UE_LOG(LogLevelNavigation, Warning, TEXT("Target Found!"));
					FoundTarget = true;
					return ConvertPath(Cells, Target);
				}
				else if (!ClosedList[WestRoom.PosX][WestRoom.PosY] && !IsDirectionBlocked(FirstPosition, EDirection::West))
				{
					GNew = Cells[FirstPosition.PosX][FirstPosition.PosY].G + 1.f;
					HNew = CalculateHeuristic(WestRoom, Target);
					FNew = GNew + HNew;

					FCell& WestCell = Cells[WestRoom.PosX][WestRoom.PosY];
					if (WestCell.IsFMax() || WestCell.F > FNew)
					{
						PPair WestRoomPair = PPair(FNew, WestRoom);
						if (!OpenList.Contains(WestRoomPair))
						{
							OpenList.Add(WestRoomPair);
						}
						WestCell.F = FNew;
						WestCell.G = GNew;
						WestCell.H = HNew;
						WestCell.Parent = FirstPosition;
					}
				}
			}
		}
	}
	if (!FoundTarget)
	{
		UE_LOG(LogLevelNavigation, Warning, TEXT("Not found path to Target"));
	}
	return TArray<FRoomPosition>();
}

class ABBLevel* UBBLevelNavigationComponent::GetCurrentLevel()
{
	return GetOwnerCharacter()->CurrentLevel;
}

bool UBBLevelNavigationComponent::IsValidRoom(FRoomPosition& RoomPosition)
{
	return IsValidRoom(RoomPosition.PosX, RoomPosition.PosY);
}

bool UBBLevelNavigationComponent::IsValidRoom(int32 PosX, int32 PosY)
{
	if (!GetCurrentLevel())
		return false;
	return PosX >= 0 && PosX < GetCurrentLevel()->FloorWidth
		&& PosY >= 0 && PosY < GetCurrentLevel()->FloorWidth;
}

bool UBBLevelNavigationComponent::IsDirectionBlocked(FRoomPosition& RoomPosition, EDirection Direction)
{
	bool IsBlocked = GetCurrentLevel()->GetRoomAt(RoomPosition)->Walls[Direction];
	//UE_LOG(LogLevelNavigation, Warning, TEXT("Wall in Direction %d in Room %d | %d"), (uint8)Direction, RoomPosition.PosX, RoomPosition.PosY);
	return IsBlocked;
}

TArray<FRoomPosition> UBBLevelNavigationComponent::ConvertPath(TArray<TArray<FCell>> Cells, FRoomPosition& Target)
{
	TArray<FRoomPosition> Path;

	int32 PosX = Target.PosX;
	int32 PosY = Target.PosY;

	while (!(Cells[PosX][PosY].Parent.PosX == PosX && Cells[PosX][PosY].Parent.PosY == PosY))
	{
		Path.Push(FRoomPosition(PosX, PosY, Target.Floor));
		FRoomPosition TempPosition = Cells[PosX][PosY].Parent;
		PosX = TempPosition.PosX;
		PosY = TempPosition.PosY;
	}

	Path.Push(FRoomPosition(PosX, PosY, Target.Floor));

	Algo::Reverse(Path);
	Path.RemoveAt(0); // Remove start room, not necessary
	return Path;
}

float UBBLevelNavigationComponent::CalculateHeuristic(FRoomPosition& RoomPosition, FRoomPosition& Target)
{
	return FMath::Abs(RoomPosition.PosX - Target.PosX) + FMath::Abs(RoomPosition.PosY - Target.PosY);
}

bool UBBLevelNavigationComponent::IsGuard()
{
	return Cast<ABBGuard>(GetOwnerCharacter());
}

ABBCharacterBase* UBBLevelNavigationComponent::GetOwnerCharacter()
{
	return Cast<ABBCharacterBase>(GetOwner());
}

ABBAIController* UBBLevelNavigationComponent::GetOwnerAIController()
{
	return Cast<ABBAIController>(GetOwnerCharacter()->GetController());
}

