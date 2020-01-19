// Fill out your copyright notice in the Description page of Project Settings.

#include "BBLootComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBLootLibrary.h"
#include "BBMainGameMode.h"

void UBBLootComponent::InitLoot(ELootType LootType)
{
	Type = LootType;
	ABBPlayerCharacterBase* Owner = GetOwnerCharacter();
	switch (Type)
	{
	case ELootType::Chihuahua:
		GetOwnerCharacter()->OnTurnStart.AddDynamic(this, &UBBLootComponent::CheckChihuahua);
		break;
	case ELootType::Mirror:
		Owner->Actions.SetMaxActions(Owner->Actions.GetMaxActions() - 1);
		break;
	case ELootType::PersianKitty:
		GetOwnerCharacter()->OnTurnStart.AddDynamic(this, &UBBLootComponent::CheckPersianKitty);
		break;
	case ELootType::Stamp:
		Owner->Actions.SetEventThreshold(1);
		break;
	case ELootType::Tiara:
		GetOwnerCharacter()->DOnRoomEntered.AddDynamic(this, &UBBLootComponent::CheckWithTiara);
		break;
	}
}

void UBBLootComponent::RemoveLoot()
{
	ABBPlayerCharacterBase* Owner = GetOwnerCharacter();
	switch (Type)
	{
	case ELootType::Chihuahua:
		GetOwnerCharacter()->OnTurnStart.RemoveDynamic(this, &UBBLootComponent::CheckChihuahua);
		break;
	case ELootType::Mirror:
		Owner->Actions.SetMaxActions(Owner->Actions.GetMaxActions() + 1);
		break;
	case ELootType::PersianKitty:
		GetOwnerCharacter()->OnTurnStart.RemoveDynamic(this, &UBBLootComponent::CheckPersianKitty);
		break;
	case ELootType::Stamp:
		Owner->Actions.SetEventThreshold(2);
		break;
	case ELootType::Tiara:
		GetOwnerCharacter()->DOnRoomEntered.RemoveDynamic(this, &UBBLootComponent::CheckWithTiara);
		break;
	}
}

class ABBPlayerCharacterBase* UBBLootComponent::GetOwnerCharacter()
{
	return Cast<ABBPlayerCharacterBase>(GetOwner());
}

void UBBLootComponent::CheckWithTiara(ABBRoomBase* RoomEntered)
{
	TArray<ABBRoomBase*> RoomsAround = UBBUtilities::GetRoomsAround(RoomEntered);
	for (ABBRoomBase* Room : RoomsAround)
	{
		if (Room->ContainsGuard())
		{
			GetOwnerCharacter()->ConsumeStealthToken();
			break;
		}
	}
}

void UBBLootComponent::CheckChihuahua()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	GameMode->RollDiceAmount = 1;
	GameMode->DOnLeaveGameAction.AddDynamic(this, &UBBLootComponent::OnChihuahuaChecked);
	GameMode->EnterRollDice();
}

void UBBLootComponent::OnChihuahuaChecked()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	GameMode->DOnLeaveGameAction.RemoveDynamic(this, &UBBLootComponent::OnChihuahuaChecked);
	if (GameMode->RollDiceResults.Contains(6))
	{
		GetOwnerCharacter()->CurrentRoom->SetAlarm(true);
	}
}

void UBBLootComponent::CheckPersianKitty()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	GameMode->RollDiceAmount = 1;
	GameMode->DOnLeaveGameAction.AddDynamic(this, &UBBLootComponent::OnPersianKittyChecked);
	GameMode->EnterRollDice();
}

void UBBLootComponent::OnPersianKittyChecked()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	GameMode->DOnLeaveGameAction.RemoveDynamic(this, &UBBLootComponent::OnPersianKittyChecked);
	if (GameMode->RollDiceResults[0] < 3)
	{
		ABBRoomBase* NearestAlarmRoom = GetNearestAlarmTile();
		NearestAlarmRoom->AddLootToRoom(ELootType::PersianKitty);
		UBBLootLibrary::DetachLootFromPlayer(ELootType::PersianKitty, GetOwnerCharacter());
	}
}

class ABBRoomBase* UBBLootComponent::GetNearestAlarmTile()
{
	ABBLevel* CurrentLevel = GetOwnerCharacter()->CurrentLevel;
	TArray<ABBRoomBase*> RoomsOfFloor = CurrentLevel->GetRoomsOfFloor(GetOwnerCharacter());
	TArray<TArray<FRoomPosition>> AllPathsToAlarmRooms;
	for (int32 Index = RoomsOfFloor.Num() - 1; Index >= 0; Index--)
	{
		ABBRoomBase* Room = RoomsOfFloor[Index];
		if (Room->IsAlarmRoom())
		{
			AllPathsToAlarmRooms.Add(GetOwnerCharacter()->GetPath(Room->Position));
		}
		else
		{
			RoomsOfFloor.RemoveAt(Index);
		}
	}
	if (RoomsOfFloor.Num() == 0)
	{
		return nullptr;
	}
	int32 NearestRoomIndex = 0;
	if (RoomsOfFloor.Num() > 1)
	{
		for (int32 Index = 1; Index < AllPathsToAlarmRooms.Num(); Index++)
		{
			if (AllPathsToAlarmRooms[Index].Num() < AllPathsToAlarmRooms[NearestRoomIndex].Num())
			{
				NearestRoomIndex = Index;
			}
		}
	}
	FRoomPosition TileTowardsNearestAlarm = AllPathsToAlarmRooms[NearestRoomIndex][0];
	return CurrentLevel->GetRoomAt(TileTowardsNearestAlarm);
}
