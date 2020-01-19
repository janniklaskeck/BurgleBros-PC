// Fill out your copyright notice in the Description page of Project Settings.

#include "BBGuard.h"
#include "BBLevel.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "UnrealMathUtility.h"
#include "DateTime.h"
#include "DrawDebugHelpers.h"
#include "BBAIController.h"
#include "BBLevelNavigationComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBLavatoryComponent.h"
#include "BBSplineDrawer.h"
#include "Components/StaticMeshComponent.h"

void ABBGuard::BeginPlay()
{
	Super::BeginPlay();
}

void ABBGuard::OnRoomEntered(ABBRoomBase* Room)
{
	Super::OnRoomEntered(Room);
	UpdatePatrolPath();
	if (Room->bContainsRaven)
	{
		Actions.ConsumeAction();
	}
	if (UBBUtilities::GetMainGameMode(this)->GetAllPlayers().Num() != 0) // Prevent on inital enter
	{
		if (Room->Position.IsSameRoom(CurrentPatrolInfo.TargetRoom))
		{
			Room->SetAlarm(false);
			DrawNextPatrolTarget();
			if (Actions.GetCurrentActions() > 0)
			{
				StartTurn();
			}
		}
	}
	if (Actions.GetCurrentActions() == 0)
	{
		FinishTurn();
	}
}

void ABBGuard::StartTurn()
{
	OnMovementStarted();
	UpdateGuardMovement();
	LevelNavigationComponent->MoveToRoomPosition(ReachableRoom);
	SelectionStaticMeshComponent->SetVisibility(true);
}

void ABBGuard::FinishTurn()
{
	OnMovementFinished();
	UBBUtilities::GetMainGameMode(GetWorld())->LeaveGuardTurn();
	OnGuardMovementEnd.Broadcast();
	SelectionStaticMeshComponent->SetVisibility(false);
}

void ABBGuard::OnMovementStarted()
{
	Actions.SetCurrentActions(Actions.GetCurrentActions() + ABBRoomBase::AmountAlarms[CurrentRoom->Position.Floor]);
	FRoomPosition AlarmRoom = GetClosestRoomWithAlarm();
	if (AlarmRoom.Floor != -1)
	{
		SetNewPatrolTarget(AlarmRoom);
		CurrentPatrolInfo.bIsAlarm = true;
		CurrentLevel->GetRoomAt(AlarmRoom)->OnAlarmChanged.Clear();
		CurrentLevel->GetRoomAt(AlarmRoom)->OnAlarmChanged.AddDynamic(this, &ABBGuard::OnTargetRoomAlarmChange);
	}
	else
	{
		CurrentPatrolInfo.bIsAlarm = false;
	}
	UpdatePatrolPath();
}

void ABBGuard::OnMovementFinished()
{
	GetBBAIController()->OnMoveSuccess.Clear();
	UpdatePatrolPath();
}

void ABBGuard::UpdateGuardMovement()
{
	TArray<FRoomPosition> Path = CurrentPatrolInfo.PathToTarget;
	if (Path.Num() == 0)
		return;
	int32 GuardMovementSpeed = Actions.GetCurrentActions();
	if (GuardMovementSpeed >= Path.Num())
	{
		int32 ValidIndex = FMath::Max(Path.Num() - 1, 0);
		ReachableRoom = Path[ValidIndex];
	}
	else
	{
		ReachableRoom = Path[GuardMovementSpeed - 1];
	}
}

void ABBGuard::OnTargetRoomAlarmChange(bool HasAlarm)
{
	if (!HasAlarm)
	{
		CurrentLevel->GetRoomAt(CurrentPatrolInfo.TargetRoom)->OnAlarmChanged.RemoveDynamic(this, &ABBGuard::OnTargetRoomAlarmChange);
		DrawNextPatrolTarget(false);
	}
}

void ABBGuard::InitCharacter(ABBLevel* Level, FRoomPosition InitialRoom)
{
	Super::InitCharacter(Level, InitialRoom);
	CurrentRoom->EnterRoom(this, true, true);
	InitPatrolDeck();
	for (int32 Index = 0; Index < PatrolDeck.Num(); Index++)
	{
		if (InitialRoom.IsSameRoom(PatrolDeck[Index]))
		{
			PatrolDeck.RemoveAt(Index);
			break;
		}
	}
	SelectionStaticMeshComponent->SetVisibility(true);
}

void ABBGuard::InitPatrolDeck()
{
	UE_LOG(LogTemp, Warning, TEXT("Init Guard Patrol Deck"));
	PatrolDeck.Empty(16);
	for (int32 X = 0; X < CurrentLevel->FloorWidth; X++)
	{
		for (int32 Y = 0; Y < CurrentLevel->FloorWidth; Y++)
		{
			PatrolDeck.Add(FRoomPosition(X, Y, CurrentRoom->Position.Floor));
		}
	}
	UBBUtilities::ShuffleArray<FRoomPosition>(PatrolDeck);
}

void ABBGuard::CheckPatrolDeck()
{
	if (PatrolDeck.Num() == 0)
	{
		VisitedRooms.Empty(16);
		Actions.SetMaxActions(Actions.GetMaxActions() + 1);
		InitPatrolDeck();
	}
}

void ABBGuard::DrawNextPatrolTarget(bool AddToVisited /*= true*/, bool RemoveFromDeck /*= true*/)
{
	if (AddToVisited)
		VisitedRooms.Add(CurrentPatrolInfo.TargetRoom);
	CheckPatrolDeck();
	SetNewPatrolTarget(PatrolDeck[0]);
	if (RemoveFromDeck)
		PatrolDeck.RemoveAt(0);
	if (CurrentPatrolInfo.TargetRoom.IsSameRoom(CurrentRoom->Position))
	{
		DrawNextPatrolTarget();
	}
	else
	{
		UpdatePatrolPath();
	}
}

void ABBGuard::DrawFirstPatrolTarget()
{
	DrawNextPatrolTarget(false, true);
}

void ABBGuard::MoveTopPatrolTargetToBottom()
{
	FRoomPosition TopPatrolTarget = PatrolDeck[0];
	PatrolDeck.RemoveAt(0);
	PatrolDeck.Add(TopPatrolTarget);
}

FRoomPosition ABBGuard::GetNextPatrolTarget()
{
	return PatrolDeck[0];
}

void ABBGuard::SetNewPatrolTarget(FRoomPosition Position)
{
	CurrentPatrolInfo.SetNewTarget(Position);
	CurrentPatrolInfo.UpdatePathToTarget(GetPathToPatrolTarget());
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	GameMode->SplineDrawer->DrawGuardPath(CurrentRoom->Position.Floor);
}

void ABBGuard::UpdatePatrolPath()
{
	SetNewPatrolTarget(CurrentPatrolInfo.TargetRoom);
}

void ABBGuard::SetCharacterHidden(bool IsHidden)
{
	Super::SetCharacterHidden(IsHidden);
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	if (IsHidden)
	{
		GameMode->SplineDrawer->ClearGuardPath(CurrentRoom->Position.Floor);
	}
	else
	{
		SetNewPatrolTarget(CurrentPatrolInfo.TargetRoom);
	}
}

TArray<FRoomPosition> ABBGuard::GetPathToPatrolTarget()
{
	return GetPath(CurrentRoom->Position, CurrentPatrolInfo.TargetRoom);
}

FRoomPosition ABBGuard::GetClosestRoomWithAlarm()
{
	TArray<ABBRoomBase*> Rooms = CurrentLevel->GetRoomsOfFloor(CurrentRoom->Position.Floor);
	int32 PathLength = 999;
	FRoomPosition ClosestRoom(-1, -1, -1);
	for (ABBRoomBase* Room : Rooms)
	{
		if (Room->bHasAlarm)
		{
			int32 PathToAlarm = GetPath(CurrentRoom->Position, Room->Position).Num();
			if (PathToAlarm < PathLength)
			{
				PathLength = PathToAlarm;
				ClosestRoom = Room->Position;
			}
		}
	}
	return ClosestRoom;
}

class ABBAIController* ABBGuard::GetBBAIController() const
{
	return Cast<ABBAIController>(GetController());
}

void ABBGuard::GiveDonuts()
{
	bHasDonuts = true;
}