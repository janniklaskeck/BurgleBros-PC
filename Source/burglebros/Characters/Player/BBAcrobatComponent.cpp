// Fill out your copyright notice in the Description page of Project Settings.

#include "BBAcrobatComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBGuard.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBLevelNavigationComponent.h"

bool UBBAcrobatComponent::CanUseAbility()
{
	if (PlayerClass == EPlayerClass::Acrobat)
	{
		ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
		ABBGuard* Guard = GameMode->GetGuard(GetOwnerCharacter()->CurrentRoom->Position.Floor);
		bool IsGuardNextToCurrentRoom = Guard->CurrentRoom->Position.IsNextToRoom(GetOwnerCharacter()->CurrentRoom->Position);
		bool NoWall = GetOwnerCharacter()->GetPath(Guard->CurrentRoom->Position).Num() == 1;
		return IsGuardNextToCurrentRoom && NoWall;
	}
	else
	{
		int32 FloorWidth = GetOwnerCharacter()->CurrentLevel->FloorWidth;
		FRoomPosition CurrentPosition = GetOwnerCharacter()->CurrentRoom->Position;
		bool XYAtZero = CurrentPosition.PosX == 0 || CurrentPosition.PosY == 0;
		bool XYAtWidth = CurrentPosition.PosX == FloorWidth - 1 || CurrentPosition.PosY == FloorWidth - 1;;
		return (XYAtZero || XYAtWidth) && GetOwnerCharacter()->Actions.GetCurrentActions() >= 3;
	}
}

bool UBBAcrobatComponent::IsActiveAbility()
{
	return true;
}

void UBBAcrobatComponent::UseAbility()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	if (PlayerClass == EPlayerClass::Acrobat)
	{
		ABBGuard* Guard = GameMode->GetGuard(GetOwnerCharacter()->CurrentRoom->Position.Floor);
		GuardRoom = Guard->CurrentRoom;
		GetOwnerCharacter()->GetLevelNavigationComponent()->MoveToRoom(GuardRoom, true, true);
		GetOwnerCharacter()->GetLevelNavigationComponent()->OnRoomEntered.AddDynamic(this, &UBBAcrobatComponent::CheckForFurtherMovement);
		GameMode->DOnEnterGuardTurn.AddDynamic(this, &UBBAcrobatComponent::CheckOnTurnEnd);
	}
	else
	{
		GameMode->EnterCharacterAction();
	}
}

void UBBAcrobatComponent::CheckForFurtherMovement(class ABBRoomBase* EnteredRoom)
{
	if (EnteredRoom != GuardRoom)
	{
		MovedAfterAction = true;
	}
}

void UBBAcrobatComponent::CheckOnTurnEnd()
{
	if (!MovedAfterAction)
	{
		GetOwnerCharacter()->ConsumeStealthToken();
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	GameMode->DOnEnterGuardTurn.RemoveDynamic(this, &UBBAcrobatComponent::CheckOnTurnEnd);
	GetOwnerCharacter()->GetLevelNavigationComponent()->OnRoomEntered.RemoveDynamic(this, &UBBAcrobatComponent::CheckForFurtherMovement);
}

void UBBAcrobatComponent::ClimbUp()
{
	GetOwnerCharacter()->Actions.ConsumeActions(GetOwnerCharacter()->Actions.GetCurrentActions());
	ABBRoomBase* CurrentRoom = GetOwnerCharacter()->CurrentRoom;
	FRoomPosition CurrentPosition = CurrentRoom->Position;
	FRoomPosition UpperPosition(CurrentPosition.PosX, CurrentPosition.PosY, CurrentPosition.Floor + 1);
	ABBRoomBase* UpperRoom = GetOwnerCharacter()->CurrentLevel->GetRoomAt(UpperPosition);
	CurrentRoom->LeaveRoom(GetOwnerCharacter());
	UpperRoom->EnterRoom(GetOwnerCharacter(), false, true);
	UpperRoom->TeleportToRoom(GetOwnerCharacter());
}

void UBBAcrobatComponent::ClimbDown()
{
	GetOwnerCharacter()->Actions.ConsumeActions(GetOwnerCharacter()->Actions.GetCurrentActions());
	ABBRoomBase* CurrentRoom = GetOwnerCharacter()->CurrentRoom;
	FRoomPosition CurrentPosition = CurrentRoom->Position;
	FRoomPosition LowerPosition(CurrentPosition.PosX, CurrentPosition.PosY, CurrentPosition.Floor - 1);
	ABBRoomBase* LowerRoom = GetOwnerCharacter()->CurrentLevel->GetRoomAt(LowerPosition);
	CurrentRoom->LeaveRoom(GetOwnerCharacter());
	LowerRoom->EnterRoom(GetOwnerCharacter(), false, true);
	LowerRoom->TeleportToRoom(GetOwnerCharacter());
}
