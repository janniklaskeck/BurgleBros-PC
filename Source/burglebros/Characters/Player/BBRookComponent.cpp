// Fill out your copyright notice in the Description page of Project Settings.

#include "BBRookComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "BBRoomBase.h"
#include "BBLevelNavigationComponent.h"
#include "BBLevel.h"

bool UBBRookComponent::CanUseAbility()
{
	if (GetOwnerCharacter()->Actions.GetCurrentActions() == 0)
	{
		return false;
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	bool HasMultiplePlayers = GameMode->GetAllPlayers().Num() > 1;
	if (!HasMultiplePlayers)
	{
		return false;
	}
	if (PlayerClass == EPlayerClass::Rook)
	{
		return !bUsedAbility;
	}
	else
	{
		return GetOwnerCharacter()->Actions.GetCurrentActions() >= GetOwnerCharacter()->Actions.GetMaxActions() && !bUsedAbility;
	}
}

bool UBBRookComponent::IsActiveAbility()
{
	return true;
}

void UBBRookComponent::UseAbility()
{
	bUsedAbility = true;
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->OnPlayerChosen.AddDynamic(this, &UBBRookComponent::ActivateAbility);
	GameMode->EnterChoosePlayer();
}

void UBBRookComponent::ActivateAbility(ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->OnPlayerChosen.RemoveDynamic(this, &UBBRookComponent::ActivateAbility);
	if (!GameMode->SelectedCharacter)
	{
		return;
	}
	if (GetOwnerCharacter()->PlayerType == EPlayerClass::Rook_Alt)
	{
		SwitchPositionWith(Character);
	}
	else
	{
		GameMode->DOnLeaveGameAction.AddDynamic(this, &UBBRookComponent::StartToMoveOtherCharacter);
	}
}

void UBBRookComponent::SwitchPositionWith(class ABBPlayerCharacterBase* OtherCharacter)
{
	if (OtherCharacter)
	{
		ABBRoomBase* CurrentRoom = GetOwnerCharacter()->CurrentRoom;
		ABBRoomBase* OtherRoom = OtherCharacter->CurrentRoom;
		OtherRoom->LeaveRoom(OtherCharacter);
		CurrentRoom->EnterRoom(OtherCharacter, true, true);
		CurrentRoom->TeleportToRoom(OtherCharacter);
		CurrentRoom->LeaveRoom(GetOwnerCharacter());
		OtherRoom->EnterRoom(GetOwnerCharacter(), true, true);
		OtherRoom->TeleportToRoom(GetOwnerCharacter());
		GetOwnerCharacter()->Actions.ConsumeAction();
	}
}

void UBBRookComponent::StartToMoveOtherCharacter()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	GameMode->DOnLeaveGameAction.RemoveDynamic(this, &UBBRookComponent::StartToMoveOtherCharacter);
	GameMode->SetValidRoomsToChoose(GetValidMoveOtherCharacterRooms());
	GameMode->OnRoomChosen.AddDynamic(this, &UBBRookComponent::OnMoveOtherCharacterRoomChosen);
	GameMode->EnterChooseRoom();
}

TArray<class ABBRoomBase*> UBBRookComponent::GetValidMoveOtherCharacterRooms()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	ABBCharacterBase* SelectedPlayer = GameMode->SelectedCharacter;
	TArray<ABBRoomBase*> ValidRooms;
	FRoomPosition OtherCharacterPosition = SelectedPlayer->CurrentRoom->Position;
	TArray<ABBRoomBase*> RoomsOnFloor = SelectedPlayer->CurrentLevel->GetRoomsOfFloor(SelectedPlayer);
	for (ABBRoomBase* Room : RoomsOnFloor)
	{
		if (Room->Position.IsNextToRoom(OtherCharacterPosition) && SelectedPlayer->GetPath(Room->Position).Num() == 1)
		{
			ValidRooms.Add(Room);
		}
	}
	if (SelectedPlayer->CurrentRoom->bHasStairsDown)
	{
		ValidRooms.Add(SelectedPlayer->CurrentLevel->GetRoomAt(SelectedPlayer->CurrentRoom->Position.GetBelowRoomPosition()));
	}
	if (SelectedPlayer->CurrentRoom->bHasStairsUp)
	{
		ValidRooms.Add(SelectedPlayer->CurrentLevel->GetRoomAt(SelectedPlayer->CurrentRoom->Position.GetAboveRoomPosition()));
	}
	return ValidRooms;
}

void UBBRookComponent::OnMoveOtherCharacterRoomChosen(class ABBRoomBase* ChosenRoom)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetOwner());
	if (ChosenRoom)
	{
		if (ChosenRoom->Position.Floor == GameMode->SelectedCharacter->CurrentRoom->Position.Floor)
		{
			GameMode->SelectedCharacter->GetLevelNavigationComponent()->MoveToRoom(ChosenRoom, false, true);
		}
		else
		{
			GameMode->SelectedCharacter->CurrentRoom->LeaveRoom(GameMode->SelectedCharacter);
			ChosenRoom->EnterRoom(GameMode->SelectedCharacter, false, true);
			ChosenRoom->TeleportToRoom(GameMode->SelectedCharacter);
		}
		GetOwnerCharacter()->Actions.ConsumeAction();
	}
	GameMode->OnRoomChosen.RemoveDynamic(this, &UBBRookComponent::OnMoveOtherCharacterRoomChosen);
}
