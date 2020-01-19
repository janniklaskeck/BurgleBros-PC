// Fill out your copyright notice in the Description page of Project Settings.

#include "BBToolLibrary.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBRoomBase.h"
#include "BBMainGameMode.h"
#include "BBLevel.h"
#include "BBComputerComponent.h"
#include "BBToolComponent.h"
#include "burglebros.h"
#include "BBLavatoryComponent.h"
#include "BBStairsComponent.h"

TArray<EToolType> UBBToolLibrary::ToolDeck;

EToolType UBBToolLibrary::LastUsedTool;

TArray<EToolType> UBBToolLibrary::ActiveTools;

void UBBToolLibrary::InitDeck()
{
	ToolDeck.Reset(13);
	ActiveTools.Reset(2);
	for (int32 Index = 0; Index < 13; Index++)
	{
		ToolDeck.Add((EToolType)Index);
	}
	UBBUtilities::ShuffleArray<EToolType>(ToolDeck);
}

void UBBToolLibrary::UseTool(class ABBPlayerCharacterBase* Character, EToolType ToolType)
{
	UE_LOG(LogTemp, Log, TEXT("Used Tool "));
	LastUsedTool = ToolType;
	switch (ToolType)
	{
	case EToolType::Blueprints:
		UseBlueprints(Character);
		break;
	case EToolType::Crowbar:
		UseCrowbar(Character);
		break;
	case EToolType::CrystalBall:
		UseCrystalBall(Character);
		break;
	case EToolType::Donuts:
		UseDonuts(Character);
		break;
	case EToolType::Dynamite:
		UseDynamite(Character);
		break;
	case EToolType::EMP:
		UseEMP(Character);
		break;
	case EToolType::InvisibleSuit:
		UseInvisibleSuit(Character);
		break;
	case EToolType::MakupKit:
		UseMakeupKit(Character);
		break;
	case EToolType::RollerSkates:
		UseRollerSkates(Character);
		break;
	case EToolType::SmokeBomb:
		UseSmokeBomb(Character);
		break;
	case EToolType::ThermalBomb:
		UseThermalBomb(Character);
		break;
	case EToolType::Virus:
		UseVirus(Character);
		break;
	default:
		break;
	}
}

EToolType UBBToolLibrary::DrawRandomToolFromDeck(ABBPlayerCharacterBase* PlayerCharacter, bool AddToPlayer /*= true*/)
{
	if (ToolDeck.Num() == 0)
	{
		InitDeck();
	}
	int32 RandomIndex = FMath::RandRange(0, ToolDeck.Num() - 1);
	EToolType RandomTool = ToolDeck[RandomIndex];
	ToolDeck.RemoveAt(RandomIndex);
	if (AddToPlayer)
		PlayerCharacter->Tools.Add(RandomTool);
	return RandomTool;
}

EToolType UBBToolLibrary::DrawRandomToolFromDeck(class ABBRoomBase* Room)
{
	if (ToolDeck.Num() == 0)
	{
		InitDeck();
	}
	int32 RandomIndex = FMath::RandRange(0, ToolDeck.Num() - 1);
	EToolType RandomTool = ToolDeck[RandomIndex];
	ToolDeck.RemoveAt(RandomIndex);
	Room->AddToolToRoom(RandomTool);
	return RandomTool;
}

bool UBBToolLibrary::GetToolFromDeck(EToolType ToolType)
{
	if (ToolDeck.Contains(ToolType))
	{
		ToolDeck.Remove(ToolType);
		return true;
	}
	return false;
}

TEnumAsByte<EToolType> UBBToolLibrary::GetLastUsedTool()
{
	return LastUsedTool;
}

void UBBToolLibrary::RemoveToolFromCharacter(EToolType Tool, class ABBPlayerCharacterBase* Character)
{
	Character->Tools.Remove(Tool);
}

void UBBToolLibrary::UseBlueprints(class ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->OnRoomChosen_NonDynamic.AddStatic(&UBBToolLibrary::OnBlueprintRoomChosen);
	TArray<ABBRoomBase*> ValidRooms = GetValidBlueprintRooms(Character);
	GameMode->SetValidRoomsToChoose(ValidRooms);
	GameMode->EnterChooseRoom();
}

TArray<class ABBRoomBase*> UBBToolLibrary::GetValidBlueprintRooms(class ABBPlayerCharacterBase* Character)
{
	TArray<ABBRoomBase*> ValidRooms;

	for (int32 Index = 0; Index < Character->CurrentLevel->AmountFloors; Index++)
	{
		for (ABBRoomBase* Room : Character->CurrentLevel->GetRoomsOfFloor(Index))
		{
			if (!Room->bIsDiscovered)
				ValidRooms.Add(Room);
		}
	}

	return ValidRooms;
}

void UBBToolLibrary::OnBlueprintRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom)
{
	if (ChosenRoom)
	{
		ChosenRoom->SetDiscovered();
		RemoveToolFromCharacter(EToolType::Blueprints, GameMode->GetCurrentPlayerCharacter());
	}
	GameMode->OnRoomChosen_NonDynamic.Clear();
}

void UBBToolLibrary::UseCrowbar(class ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->OnRoomChosen_NonDynamic.AddStatic(&UBBToolLibrary::OnCrowBarRoomChosen);
	TArray<ABBRoomBase*> ValidRooms = GetValidCrowBarRooms(Character);
	GameMode->SetValidRoomsToChoose(ValidRooms);
	GameMode->EnterChooseRoom();
}

TArray<ABBRoomBase*> UBBToolLibrary::GetValidCrowBarRooms(class ABBPlayerCharacterBase* Character)
{
	TArray<ABBRoomBase*> ValidRooms;
	FRoomPosition Origin = Character->CurrentRoom->Position;
	ABBRoomBase* North = Character->CurrentLevel->GetRoomAt(Origin.PosX, Origin.PosY + 1, Origin.Floor);
	if (North && North->GetRoomFunction()->CanDisableRoom())
		ValidRooms.Add(North);
	ABBRoomBase* East = Character->CurrentLevel->GetRoomAt(Origin.PosX + 1, Origin.PosY, Origin.Floor);
	if (East && East->GetRoomFunction()->CanDisableRoom())
		ValidRooms.Add(East);
	ABBRoomBase* South = Character->CurrentLevel->GetRoomAt(Origin.PosX, Origin.PosY - 1, Origin.Floor);
	if (South && South->GetRoomFunction()->CanDisableRoom())
		ValidRooms.Add(South);
	ABBRoomBase* West = Character->CurrentLevel->GetRoomAt(Origin.PosX - 1, Origin.PosY, Origin.Floor);
	if (West && West->GetRoomFunction()->CanDisableRoom())
		ValidRooms.Add(West);
	return ValidRooms;
}

void UBBToolLibrary::OnCrowBarRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom)
{
	if (ChosenRoom)
	{
		ChosenRoom->GetRoomFunction()->bIsRoomEnabled = false;
		RemoveToolFromCharacter(EToolType::Crowbar, GameMode->GetCurrentPlayerCharacter());
	}
	GameMode->OnRoomChosen_NonDynamic.Clear();
}

void UBBToolLibrary::UseCrystalBall(class ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->EnterTool();
}

void UBBToolLibrary::UseDonuts(class ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->EnterTool();
}

void UBBToolLibrary::UseDynamite(class ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->EnterTool();
}

bool UBBToolLibrary::CanUseDynamite(class ABBRoomBase* Room, EDirection WallSide)
{
	bool HasNeighbourRoom = Room->CurrentLevel->GetNeighbourRoomInDirection(Room, WallSide);
	bool HasWallInDirection = Room->Walls[WallSide];
	return HasNeighbourRoom && HasWallInDirection;
}

void UBBToolLibrary::RemoveDynamiteWall(class ABBRoomBase* Room, EDirection WallSide)
{
	if (!Room)
		return;
	ABBRoomBase* NeighbourRoom = Room->CurrentLevel->GetNeighbourRoomInDirection(Room, WallSide);
	Room->SetWall(WallSide, false);
	NeighbourRoom->SetWall(UBBUtilities::GetOppositeDirection(WallSide), false);
	Room->SetAlarm(true);
	RemoveToolFromCharacter(EToolType::Dynamite, UBBUtilities::GetMainGameMode(Room)->GetCurrentPlayerCharacter());
	EndChooseTool(Room);
}

void UBBToolLibrary::UseEMP(class ABBPlayerCharacterBase* Character)
{
	for (int32 Index = 0; Index < Character->CurrentLevel->AmountFloors; Index++)
	{
		TArray<ABBRoomBase*> RoomsOfFloor = Character->CurrentLevel->GetRoomsOfFloor(Index);
		for (ABBRoomBase* Room : RoomsOfFloor)
		{
			Room->SetAlarm(false);
		}
	}
	UBBToolComponent* Component = NewObject<UBBToolComponent>(Character, UBBToolComponent::StaticClass(), FName(*GETENUMSTRING("EToolType", EToolType::EMP)));
	Component->RegisterComponent();
	Component->InitTool(EToolType::EMP);
	RemoveToolFromCharacter(EToolType::EMP, Character);
	EndChooseTool(Character);
}

void UBBToolLibrary::UseInvisibleSuit(class ABBPlayerCharacterBase* Character)
{
	Character->Actions.AddOneAction();
	Cast<ABBPlayerCharacterBase>(Character)->bIsInvisible = true;
	RemoveToolFromCharacter(EToolType::InvisibleSuit, Character);
	EndChooseTool(Character);
}

void UBBToolLibrary::UseMakeupKit(class ABBPlayerCharacterBase* Character)
{
	for (ABBCharacterBase* CharacterInRoom : Character->CurrentRoom->CharactersInRoom)
	{
		ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(CharacterInRoom);
		if (PlayerCharacter)
			PlayerCharacter->AddStealthToken();
	}
	RemoveToolFromCharacter(EToolType::MakupKit, Character);
	EndChooseTool(Character);
}

void UBBToolLibrary::UseRollerSkates(class ABBPlayerCharacterBase* Character)
{
	Character->Actions.SetCurrentActions(Character->Actions.GetCurrentActions() + 2);
	RemoveToolFromCharacter(EToolType::RollerSkates, Character);
	EndChooseTool(Character);
}

void UBBToolLibrary::UseSmokeBomb(class ABBPlayerCharacterBase* Character)
{
	ABBRoomBase* CurrentRoom = Character->CurrentRoom;
	CurrentRoom->AmountStealthTokens += 3;
	RemoveToolFromCharacter(EToolType::SmokeBomb, Character);
	EndChooseTool(Character);
}

void UBBToolLibrary::UseThermalBomb(class ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->EnterTool();
}

bool UBBToolLibrary::CanUseThermalBomb(class ABBRoomBase* Room, bool UseUp)
{
	if (Room->bHasStairsDown || Room->bHasStairsUp)
		return false;
	if (!UseUp)
	{
		return Room->Position.Floor > 0;
	}
	return true;
}

void UBBToolLibrary::MakeThermalBombStairs(class ABBRoomBase* Room, bool StairsGoUp)
{
	Room->SetAlarm(true);
	if (StairsGoUp)
	{
		FRoomPosition RoomAbovePosition = Room->Position.GetAboveRoomPosition();
		ABBRoomBase* RoomAbove = Room->CurrentLevel->GetRoomAt(RoomAbovePosition);
		if (RoomAbove)
		{
			RoomAbove->SetHasStairsDown(true);
		}
		Room->SetHasStairsUp(true);
	}
	else
	{
		Room->SetHasStairsDown(true);
	}
	RemoveToolFromCharacter(EToolType::ThermalBomb, UBBUtilities::GetMainGameMode(Room)->GetCurrentPlayerCharacter());
	EndChooseTool(Room);
}

void UBBToolLibrary::UseVirus(class ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->OnRoomChosen_NonDynamic.Clear();
	GameMode->OnRoomChosen_NonDynamic.AddStatic(&UBBToolLibrary::OnVirusRoomChosen);
	TArray<ABBRoomBase*> ValidRooms = GetValidVirusRooms(Character);
	GameMode->SetValidRoomsToChoose(ValidRooms);
	GameMode->EnterChooseRoom();
}

TArray<ABBRoomBase*> UBBToolLibrary::GetValidVirusRooms(class ABBPlayerCharacterBase* Character)
{
	TArray<ABBRoomBase*> ValidRooms;
	TArray<ABBRoomBase*> ComputerFingerprint = Character->CurrentLevel->GetRoomsOfType(ERoomType::ComputerFingerprint);
	if (ComputerFingerprint.Num() > 0 && ComputerFingerprint[0]->bIsDiscovered)
		ValidRooms.Add(ComputerFingerprint[0]);

	TArray<ABBRoomBase*> ComputerLaser = Character->CurrentLevel->GetRoomsOfType(ERoomType::ComputerLaser);
	if (ComputerLaser.Num() > 0 && ComputerLaser[0]->bIsDiscovered)
		ValidRooms.Add(ComputerLaser[0]);

	TArray<ABBRoomBase*> ComputerMotion = Character->CurrentLevel->GetRoomsOfType(ERoomType::ComputerMotion);
	if (ComputerMotion.Num() > 0 && ComputerMotion[0]->bIsDiscovered)
		ValidRooms.Add(ComputerMotion[0]);
	return ValidRooms;
}

void UBBToolLibrary::OnVirusRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom)
{
	if (ChosenRoom)
	{
		ChosenRoom->GetRoomFunctionAs<UBBComputerComponent>()->AddTokens(3);
		RemoveToolFromCharacter(EToolType::Virus, GameMode->GetCurrentPlayerCharacter());
	}
	GameMode->OnRoomChosen_NonDynamic.Clear();
}

void UBBToolLibrary::EndChooseTool(UObject* WorldContext)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(WorldContext);
	GameMode->LeaveGameAction();
}
