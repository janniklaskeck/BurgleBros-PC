// Fill out your copyright notice in the Description page of Project Settings.

#include "BBPlayerCharacterBase.h"
#include "BBRoomBase.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "burglebros.h"
#include "BBLevelNavigationComponent.h"
#include "BBLevel.h"
#include "BBEventLibrary.h"
#include "BBSafeComponent.h"

void ABBPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABBPlayerCharacterBase::StartTurn()
{
	Super::StartTurn();
	if (bIsInvisible)
		bIsInvisible = false;
}

class UBBPlayerClassComponent* ABBPlayerCharacterBase::GetClassComponent() const
{
	return ClassComponent;
}

void ABBPlayerCharacterBase::InitCharacter(ABBLevel* Level, FRoomPosition InitialRoom)
{
	Super::InitCharacter(Level, InitialRoom);
	CurrentRoom->EnterRoom(this, true, true);
	StealthTokens = 3;
}

void ABBPlayerCharacterBase::InitPlayerClass(EPlayerClass ActualPlayerClass)
{
	PlayerType = ActualPlayerClass;
	ClassComponent = NewObject<UBBPlayerClassComponent>(this, PlayerClass, "PlayerClassComponent");
	ClassComponent->RegisterComponent();
}

int32 ABBPlayerCharacterBase::GetStealthTokens()
{
	return StealthTokens;
}

void ABBPlayerCharacterBase::AddStealthToken()
{
	++StealthTokens;
}

void ABBPlayerCharacterBase::ConsumeStealthToken()
{
	if (StealthTokens == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Lost!"));
		UBBUtilities::GetMainGameMode(this)->LoseGame();
	}
	else
	{
		--StealthTokens;
	}
}

void ABBPlayerCharacterBase::AddLootComponent(ELootType Type)
{
	Loot.Add(Type);
	UBBLootComponent* Component = NewObject<UBBLootComponent>(this, UBBLootComponent::StaticClass(), FName(*GETENUMSTRING("ELootType", Type)));
	Component->RegisterComponent();
	Component->InitLoot(Type);
	OnLootChanged.Broadcast();
}

void ABBPlayerCharacterBase::RemoveLootComponent(ELootType Type)
{
	Loot.Remove(Type);
	TArray<UActorComponent*> LootComponents = GetComponentsByClass(UBBLootComponent::StaticClass());
	for (UActorComponent* Component : LootComponents)
	{
		UBBLootComponent* LootComponent = Cast<UBBLootComponent>(Component);
		if (LootComponent->Type == Type)
		{
			LootComponent->RemoveLoot();
			Component->DestroyComponent();
			break;
		}
	}
	OnLootChanged.Broadcast();
}

bool ABBPlayerCharacterBase::CanMoveToSelectedRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* SelectedRoom = GameMode->GetSelectedRoom();
	if (!SelectedRoom || !SelectedRoom->IsSameFloor(CurrentRoom))
	{
		return false;
	}
	else if (SelectedRoom->RoomType == ERoomType::SecretDoor && SelectedRoom->Position.IsNextToRoom(CurrentRoom->Position))
	{
		return SelectedRoom->GetRoomFunction()->CanEnter(this);
	}
	return GetLevelNavigationComponent()->CanMoveToRoom(SelectedRoom);
}

bool ABBPlayerCharacterBase::CanMoveToUpperRoom()
{
	if (!CurrentRoom->bHasStairsUp || UBBEventLibrary::ActiveEvents.Contains(EEventType::TimeLock))
		return false;
	FRoomPosition UpperRoomPosition = CurrentRoom->Position.GetAboveRoomPosition();
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* RoomAbove = GameMode->GetCurrentLevel()->GetRoomAt(UpperRoomPosition);
	if (RoomAbove)
	{
		return RoomAbove->GetRoomFunction()->CanEnter(this);
	}
	else
	{
		return CanMoveToRoof();
	}
}

bool ABBPlayerCharacterBase::CanMoveToLowerRoom()
{
	if (!CurrentRoom->bHasStairsDown || UBBEventLibrary::ActiveEvents.Contains(EEventType::TimeLock))
		return false;
	FRoomPosition LowerRoomPosition = CurrentRoom->Position.GetBelowRoomPosition();
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* RoomBelow = GameMode->GetCurrentLevel()->GetRoomAt(LowerRoomPosition);
	return RoomBelow && RoomBelow->GetRoomFunction()->CanEnter(this);
}

bool ABBPlayerCharacterBase::CanPeekSelectedRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* SelectedRoom = GameMode->GetSelectedRoom();
	if (!SelectedRoom || !SelectedRoom->IsSameFloor(CurrentRoom))
	{
		return false;
	}
	return SelectedRoom->GetRoomFunction()->CanPeekRoom(this);
}

bool ABBPlayerCharacterBase::CanPeekUpperRoom()
{
	if (!CurrentRoom->bHasStairsUp)
	{
		return false;
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* UpperRoom = GameMode->GetCurrentLevel()->GetRoomAt(CurrentRoom->Position.GetAboveRoomPosition());
	return UpperRoom && UpperRoom->GetRoomFunction()->CanPeekRoom(this, true);
}

bool ABBPlayerCharacterBase::CanPeekLowerRoom()
{
	if (!CurrentRoom->bHasStairsDown)
	{
		return false;
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* LowerRoom = GameMode->GetCurrentLevel()->GetRoomAt(CurrentRoom->Position.GetBelowRoomPosition());
	return LowerRoom && LowerRoom->GetRoomFunction()->CanPeekRoom(this, true);
}

bool ABBPlayerCharacterBase::CanUseSelectedRoom()
{
	return CurrentRoom->GetRoomFunction()->CanUse(this) && CurrentRoom->CharactersInRoom.Contains(this);
}

bool ABBPlayerCharacterBase::CanUseTool()
{
	if (Loot.Contains(ELootType::Bust))
		return false;
	if (Tools.Num() == 1 && Tools.Contains(EToolType::Stethoscope))
		return false;
	return Tools.Num() > 0;
}

bool ABBPlayerCharacterBase::CanMoveToRoof()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBLevel* Level = GameMode->GetCurrentLevel();
	TArray<ABBRoomBase*> SafeRooms = Level->GetRoomsOfType(ERoomType::Safe);
	bool AllSafesCracked = true;
	for (ABBRoomBase* Room : SafeRooms)
	{
		if (!Room->GetRoomFunctionAs<UBBSafeComponent>()->bIsCracked)
		{
			AllSafesCracked = false;
			break;
		}
	}
	for (int32 Index = 0; Index < Level->AmountFloors; Index++)
	{
		TArray<ABBRoomBase*> FloorRooms = Level->GetRoomsOfFloor(Index);
		for (ABBRoomBase* Room : FloorRooms)
		{
			if (Room->LootInRoom.Num() > 0)
			{
				return false;
			}
		}
	}
	return AllSafesCracked;
}

void ABBPlayerCharacterBase::MoveToRoof()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	GameMode->TeleportToRoof(this);
	bool AllPlayersEscaped = true;
	for (ABBPlayerCharacterBase* Player : GameMode->GetAllPlayers())
	{
		if (!Player->bIsEscaped)
		{
			AllPlayersEscaped = false;
			break;
		}
	}
	if (AllPlayersEscaped)
	{
		GameMode->WinGame();
	}
}

void ABBPlayerCharacterBase::MoveToSelectedRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* SelectedRoom = GameMode->GetSelectedRoom();
	if (SelectedRoom->RoomType == ERoomType::SecretDoor && SelectedRoom->Position.IsNextToRoom(CurrentRoom->Position) && GetPath(SelectedRoom->Position).Num() > 1)
	{
		CurrentRoom->LeaveRoom(this);
		SelectedRoom->EnterRoom(this);
		SelectedRoom->TeleportToRoom(this);
	}
	else
	{
		GetLevelNavigationComponent()->MoveToRoom(SelectedRoom);
	}
}

void ABBPlayerCharacterBase::MoveToLowerRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	bool IsWalkway = CurrentRoom->RoomType == ERoomType::Walkway;
	FRoomPosition BelowPosition = CurrentRoom->Position.GetBelowRoomPosition();
	ABBRoomBase* RoomBelow = GameMode->GetCurrentLevel()->GetRoomAt(BelowPosition);
	CurrentRoom->LeaveRoom(this);
	RoomBelow->EnterRoom(this, IsWalkway);
	RoomBelow->TeleportToRoom(this);
}

void ABBPlayerCharacterBase::MoveToUpperRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	FRoomPosition AbovePosition = CurrentRoom->Position.GetAboveRoomPosition();
	ABBRoomBase* RoomAbove = GameMode->GetCurrentLevel()->GetRoomAt(AbovePosition);
	if (RoomAbove)
	{
		CurrentRoom->LeaveRoom(this);
		RoomAbove->EnterRoom(this);
		RoomAbove->TeleportToRoom(this);
	}
	else
	{
		MoveToRoof();
	}
}

void ABBPlayerCharacterBase::PeekSelectedRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* SelectedRoom = GameMode->GetSelectedRoom();
	if (!SelectedRoom)
		return;
	SelectedRoom->GetRoomFunction()->PeekRoom(this);
}

void ABBPlayerCharacterBase::PeekUpperRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* UpperRoom = GameMode->GetCurrentLevel()->GetRoomAt(CurrentRoom->Position.GetAboveRoomPosition());
	if (!UpperRoom)
		return;
	UpperRoom->GetRoomFunction()->PeekRoom(this);
}

void ABBPlayerCharacterBase::PeekLowerRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* LowerRoom = GameMode->GetCurrentLevel()->GetRoomAt(CurrentRoom->Position.GetBelowRoomPosition());
	if (!LowerRoom)
		return;
	LowerRoom->GetRoomFunction()->PeekRoom(this);
}

void ABBPlayerCharacterBase::UseSelectedRoom()
{
	CurrentRoom->GetRoomFunction()->OnUse(this);
}

void ABBPlayerCharacterBase::UseCharacterAbility()
{
	ClassComponent->UseAbility();
}
