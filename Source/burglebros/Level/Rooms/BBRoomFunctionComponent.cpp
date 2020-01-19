// Fill out your copyright notice in the Description page of Project Settings.

#include "BBRoomFunctionComponent.h"
#include "BBRoomBase.h"
#include "BBGuard.h"
#include "BBPlayerCharacterBase.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"

UBBRoomFunctionComponent::UBBRoomFunctionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBBRoomFunctionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBBRoomFunctionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int32 UBBRoomFunctionComponent::GetEnterCost(class ABBCharacterBase* Character)
{
	ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
	if (PlayerCharacter)
	{
		bool HasGemstone = PlayerCharacter->Loot.Contains(ELootType::Gemstone);
		bool OtherPlayersPreset = GetOwnerRoom()->CharactersInRoom.ContainsByPredicate([](ABBCharacterBase* Character)
		{
			return Cast<ABBPlayerCharacterBase>(Character);
		});
		if (HasGemstone && OtherPlayersPreset)
		{
			return 2;
		}
	}
	return 1;
}

bool UBBRoomFunctionComponent::CanEnter(class ABBCharacterBase* Character)
{
	if (IsGuard(Character))
	{
		return Character->Actions.GetCurrentActions() > 0;
	}
	return Character->Actions.GetCurrentActions() >= GetEnterCost(Character);
}

bool UBBRoomFunctionComponent::CanDisableRoom()
{
	return false;
}

bool UBBRoomFunctionComponent::OnEnter(class ABBCharacterBase* Character, bool IgnoreEffects /*= false*/, bool IgnoreCost /*= false*/)
{
	if (!IgnoreCost)
	{
		if (IsGuard(Character) || !bIsRoomEnabled)
		{
			Character->Actions.ConsumeAction();
		}
		else
		{
			Character->Actions.ConsumeActions(GetEnterCost(Character));
		}
	}
	bool CanEnterRoom = true;
	Character->OnRoomEntered(GetOwnerRoom());
	if (!IgnoreEffects)
	{
		if (bIsRoomEnabled)
			CanEnterRoom = OnEntered(Character);
		if (CanEnterRoom)
		{
			for (ABBCharacterBase* CharacterInRoom : GetOwnerRoom()->CharactersInRoom)
			{
				if (IsGuard(CharacterInRoom))
				{
					ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
					if (PlayerCharacter && !PlayerCharacter->bIsInvisible)
					{
						if (GetOwnerRoom()->AmountStealthTokens > 0)
						{
							GetOwnerRoom()->AmountStealthTokens -= 1;
						}
						else
						{
							PlayerCharacter->ConsumeStealthToken();
						}
					}
				}
				else if (IsGuard(Character))
				{
					ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(CharacterInRoom);
					if (PlayerCharacter && !PlayerCharacter->bIsInvisible)
					{
						if (GetOwnerRoom()->AmountStealthTokens > 0)
						{
							GetOwnerRoom()->AmountStealthTokens -= 1;
						}
						else
						{
							PlayerCharacter->ConsumeStealthToken();
						}
					}
				}
			}
		}
	}
	return CanEnterRoom;
}

bool UBBRoomFunctionComponent::OnEntered(class ABBCharacterBase* Character)
{
	return true;
}

void UBBRoomFunctionComponent::OnLeave(class ABBCharacterBase* Character)
{
	Character->OnRoomLeft(GetOwnerRoom());
	OnLeft(Character);
}

void UBBRoomFunctionComponent::OnLeft(class ABBCharacterBase* Character)
{

}

bool UBBRoomFunctionComponent::CanUse(class ABBCharacterBase* Character)
{
	if (!bIsRoomEnabled)
		return false;
	ERoomType RoomType = GetOwnerRoom()->RoomType;
	if (RoomType == ERoomType::Safe
		|| RoomType == ERoomType::ComputerFingerprint
		|| RoomType == ERoomType::ComputerLaser
		|| RoomType == ERoomType::ComputerMotion
		|| RoomType == ERoomType::ServiceDuct
		|| RoomType == ERoomType::Stairs)
	{
		return GetOwnerRoom()->bIsDiscovered && Character->Actions.GetCurrentActions() > 0;
	}
	return false;
}

void UBBRoomFunctionComponent::OnUse(class ABBCharacterBase* Character)
{
	OnUsed(Character);
}

void UBBRoomFunctionComponent::PeekRoom(class ABBCharacterBase* Character)
{
	GetOwnerRoom()->SetDiscovered();
	Character->Actions.ConsumeAction();
}

bool UBBRoomFunctionComponent::CanPeekRoom(class ABBCharacterBase* Character, bool IgnoreFloors /*= false*/)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	ABBRoomBase* OwnerRoom = GetOwnerRoom();
	if (!OwnerRoom || OwnerRoom->bIsDiscovered || !UBBUtilities::GetMainGameMode(GetWorld())->IsPlayerTurn())
		return false;
	ABBRoomBase* CurrentRoom = Character->CurrentRoom;
	bool SameFloor = CurrentRoom->Position.Floor == OwnerRoom->Position.Floor;
	bool HasActionsLeft = Character->Actions.HasActionsLeft();
	bool RoomInRange = true;
	if (SameFloor)
	{
		RoomInRange = Character->GetPath(OwnerRoom->Position).Num() == 1;
	}

	return HasActionsLeft && (IgnoreFloors || (SameFloor && RoomInRange));
}

void UBBRoomFunctionComponent::OnUsed(class ABBCharacterBase* Character)
{

}

ABBRoomBase* UBBRoomFunctionComponent::GetOwnerRoom()
{
	return Cast<ABBRoomBase>(GetOwner());
}

bool UBBRoomFunctionComponent::IsGuard(class ABBCharacterBase* Character)
{
	return Cast<ABBGuard>(Character);
}
