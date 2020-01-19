// Fill out your copyright notice in the Description page of Project Settings.

#include "BBEventLibrary.h"
#include "BBUtilities.h"
#include "BBPlayerCharacterBase.h"
#include "BBGuard.h"
#include "BBMainGameMode.h"
#include "BBRoomBase.h"
#include "BBLevel.h"
#include "BBToolLibrary.h"
#include "BBComputerComponent.h"
#include "BBKeypadComponent.h"
#include "BBLaboratoryComponent.h"
#include "BBLevelNavigationComponent.h"
#include "BBEventComponent.h"
#include "burglebros.h"

TArray<EEventType> UBBEventLibrary::EventDeck;

EEventType UBBEventLibrary::LastEvent;

TArray<EEventType> UBBEventLibrary::ActiveEvents;

void UBBEventLibrary::InitEventDeck()
{
	EventDeck.Reset(32);
	ActiveEvents.Reset(2);
	for (int32 Index = 0; Index < 26; Index++)
	{
		EventDeck.Add((EEventType)Index);
	}
	UBBUtilities::ShuffleArray<EEventType>(EventDeck);
}

void UBBEventLibrary::ExecuteEvent(EEventType Type, class ABBCharacterBase* Character, bool RemoveFromDeck /*= true*/)
{
	if (EventDeck.Num() == 0)
	{
		InitEventDeck();
	}
	if (RemoveFromDeck)
		EventDeck.Remove(Type);
	LastEvent = Type;
	UE_LOG(LogTemp, Warning, TEXT("ExecuteEvent: %s"), *GETENUMSTRING("EEventType", Type));
	switch (Type)
	{
	case EEventType::BrownOut:
		ExecuteBrownOut(Character);
		break;
	case EEventType::BuddySystem:
		ExecuteBuddySystem(Character);
		break;
	case EEventType::ChangeOfPlans:
		ExecuteChangeOfPlans(Character);
		break;
	case EEventType::Crash:
		ExecuteCrash(Character);
		break;
	case EEventType::Daydreaming:
		ExecuteDaydreaming(Character);
		break;
	case EEventType::DeadDrop:
		ExecuteDeadDrop(Character);
		break;
	case EEventType::Espresso:
		ExecuteEspresso(Character);
		break;
	case EEventType::FreightElevator:
		ExecuteFreightElevator(Character);
		break;
	case EEventType::GoWithYourGut:
		ExecuteGoWithYourGut(Character);
		break;
	case EEventType::Gymnastics:
		ExecuteGymnastics(Character);
		break;
	case EEventType::HeadsUp:
		ExecuteHeadsUp(Character);
		break;
	case EEventType::JumpTheGun:
		ExecuteJumpTheGun(Character);
		break;
	case EEventType::Jury_Rig:
		ExecuteJuryRig(Character);
		break;
	case EEventType::KeycodeChange:
		ExecuteKeycodeChange(Character);
		break;
	case EEventType::Lampshade:
		ExecuteLampshade(Character);
		break;
	case EEventType::LostGrip:
		ExecuteLostGrip(Character);
		break;
	case EEventType::Peekhole:
		ExecutePeekhole(Character);
		break;
	case EEventType::Reboot:
		ExecuteReboot(Character);
		break;
	case EEventType::ShiftChange:
		ExecuteShiftChange(Character);
		break;
	case EEventType::Shoplifting:
		ExecuteShoplifting(Character);
		break;
	case EEventType::Squeak:
		ExecuteSqueak(Character);
		break;
	case EEventType::SwitchSigns:
		ExecuteSwitchSigns(Character);
		break;
	case EEventType::ThrowVoice:
		ExecuteThrowVoice(Character);
		break;
	case EEventType::TimeLock:
		ExecuteTimeLock(Character);
		break;
	case EEventType::VideoLoop:
		ExecuteVideoLoop(Character);
		break;
	case EEventType::WhereIsHe:
		ExecuteWhereIsHe(Character);
		break;
	default:
		break;
	}
}

TArray<EEventType> UBBEventLibrary::GetFirstThreeEvents()
{
	TArray<EEventType> Events;
	Events.Add(EventDeck[0]);
	Events.Add(EventDeck[1]);
	Events.Add(EventDeck[2]);
	return Events;
}

void UBBEventLibrary::InsertEventsOnTop(TArray<EEventType> Events, bool RemoveDuplicatesFirst /*= true*/)
{
	if (RemoveDuplicatesFirst)
	{
		for (EEventType Type : Events)
		{
			EventDeck.Remove(Type);
		}
	}
	for (int32 Index = Events.Num() - 1; Index >= 0; Index--)
	{
		EventDeck.Insert(Events[Index], 0);
	}
}

void UBBEventLibrary::MoveFirstEventToBottom()
{
	EEventType FirstEvent = EventDeck[0];
	EventDeck.RemoveAt(0);
	EventDeck.Insert(FirstEvent, EventDeck.Num());
}

EEventType UBBEventLibrary::GetTopEvent()
{
	return EventDeck[0];
}

EEventType UBBEventLibrary::GetLastEvent()
{
	return LastEvent;
}

void UBBEventLibrary::ExecuteWhereIsHe(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	ABBGuard* CurrentGuard = GameMode->GetCurrentGuardPawn();
	ABBRoomBase* TargetRoom = Character->CurrentLevel->GetRoomAt(CurrentGuard->CurrentPatrolInfo.TargetRoom);
	CurrentGuard->CurrentRoom->LeaveRoom(CurrentGuard);
	TargetRoom->EnterRoom(CurrentGuard, true, true);
	TargetRoom->TeleportToRoom(CurrentGuard);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteEspresso(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character->GetWorld());
	ABBGuard* CurrentGuard = GameMode->GetCurrentGuardPawn();
	CurrentGuard->Actions.SetCurrentActions(CurrentGuard->Actions.GetCurrentActions() + 1);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteReboot(class ABBCharacterBase* Character)
{
	TArray<ABBRoomBase*> ComputerFingerprint = Character->CurrentLevel->GetRoomsOfType(ERoomType::ComputerFingerprint);
	TArray<ABBRoomBase*> ComputerLaser = Character->CurrentLevel->GetRoomsOfType(ERoomType::ComputerLaser);
	TArray<ABBRoomBase*> ComputerMotion = Character->CurrentLevel->GetRoomsOfType(ERoomType::ComputerMotion);
	if (ComputerFingerprint.Num() > 0)
	{
		ABBRoomBase* ComputerRoom = ComputerFingerprint[0];
		if (ComputerRoom->bIsDiscovered)
		{
			ComputerRoom->GetRoomFunctionAs<UBBComputerComponent>()->AmountTokens = 1;
		}
	}
	if (ComputerLaser.Num() > 0)
	{
		ABBRoomBase* ComputerRoom = ComputerLaser[0];
		if (ComputerRoom->bIsDiscovered)
		{
			ComputerRoom->GetRoomFunctionAs<UBBComputerComponent>()->AmountTokens = 1;
		}
	}
	if (ComputerMotion.Num() > 0)
	{
		ABBRoomBase* ComputerRoom = ComputerMotion[0];
		if (ComputerRoom->bIsDiscovered)
		{
			ComputerRoom->GetRoomFunctionAs<UBBComputerComponent>()->AmountTokens = 1;
		}
	}
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteKeycodeChange(class ABBCharacterBase* Character)
{
	TArray<ABBRoomBase*> KeypadRooms = Character->CurrentLevel->GetRoomsOfType(ERoomType::Keypad);

	for (ABBRoomBase* Room : KeypadRooms)
	{
		Room->GetRoomFunctionAs<UBBKeypadComponent>()->bIsUnlocked = false;
	}

	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteDaydreaming(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	ABBGuard* CurrentGuard = GameMode->GetCurrentGuardPawn();
	CurrentGuard->Actions.SetCurrentActions(CurrentGuard->Actions.GetCurrentActions() - 1);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteFreightElevator(class ABBCharacterBase* Character)
{
	if (Character->CurrentRoom->Position.Floor + 1 == Character->CurrentLevel->AmountFloors)
	{
		EndEventCheck(Character);
		return;
	}
	FRoomPosition CurrentPosition = Character->CurrentRoom->Position;
	FRoomPosition TargetPosition = FRoomPosition(CurrentPosition.PosX, CurrentPosition.PosY, CurrentPosition.Floor + 1);
	Character->CurrentRoom->LeaveRoom(Character);
	ABBRoomBase* TargetRoom = Character->CurrentLevel->GetRoomAt(TargetPosition);
	TargetRoom->EnterRoom(Character, true, true);
	TargetRoom->TeleportToRoom(Character);

	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteLostGrip(class ABBCharacterBase* Character)
{
	if (Character->CurrentRoom->Position.Floor - 1 < 0)
	{
		EndEventCheck(Character);
		return;
	}
	FRoomPosition CurrentPosition = Character->CurrentRoom->Position;
	FRoomPosition TargetPosition = FRoomPosition(CurrentPosition.PosX, CurrentPosition.PosY, CurrentPosition.Floor - 1);
	Character->CurrentRoom->LeaveRoom(Character);
	ABBRoomBase* TargetRoom = Character->CurrentLevel->GetRoomAt(TargetPosition);
	TargetRoom->EnterRoom(Character, true, true);
	TargetRoom->TeleportToRoom(Character);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteJuryRig(class ABBCharacterBase* Character)
{
	UBBUtilities::GetMainGameMode(Character)->EnterDrawCard();
}

void UBBEventLibrary::ExecuteShoplifting(class ABBCharacterBase* Character)
{
	TArray<ABBRoomBase*> LaboratoryRooms = Character->CurrentLevel->GetRoomsOfType(ERoomType::Laboratory);
	for (ABBRoomBase* Room : LaboratoryRooms)
	{
		if (Room->GetRoomFunctionAs<UBBLaboratoryComponent>()->bAlreadyVisited)
		{
			Room->SetAlarm(true);
		}
	}
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteLampshade(class ABBCharacterBase* Character)
{
	ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
	PlayerCharacter->AddStealthToken();
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteCrash(class ABBCharacterBase* Character)
{
	for (ABBRoomBase* Room : Character->CurrentLevel->GetRoomsOfFloor(Character->CurrentRoom->Position.Floor))
	{
		if (Room->bHasAlarm)
		{
			EndEventCheck(Character);
			return;
		}
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	ABBGuard* CurrentGuard = GameMode->GetCurrentGuardPawn();
	CurrentGuard->SetNewPatrolTarget(Character->CurrentRoom->Position);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteDeadDrop(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	if (GameMode->GetAllPlayers().Num() == 1)
	{
		EndEventCheck(Character);
		return;
	}
	int32 NextPlayerIndex = GameMode->GetNextPlayerIndex();
	if (NextPlayerIndex != GameMode->GetCurrentPlayerIndex())
	{
		ABBPlayerCharacterBase* CurrentPlayer = Cast<ABBPlayerCharacterBase>(Character);
		ABBPlayerCharacterBase* NextPlayer = Cast<ABBPlayerCharacterBase>(GameMode->GetAllPlayers()[NextPlayerIndex]);
		NextPlayer->Tools.Append(CurrentPlayer->Tools);
		CurrentPlayer->Tools.Reset(4);
		for (ELootType Loot : CurrentPlayer->Loot)
		{
			NextPlayer->AddLootComponent(Loot);
			CurrentPlayer->RemoveLootComponent(Loot);
		}
	}
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteSwitchSigns(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	ABBGuard* CurrentGuard = GameMode->GetCurrentGuardPawn();
	FRoomPosition CurrentGuardPosition = CurrentGuard->CurrentRoom->Position;
	FRoomPosition CurrentGuardPatrolTarget = CurrentGuard->CurrentPatrolInfo.TargetRoom;
	CurrentGuard->SetNewPatrolTarget(CurrentGuardPosition);
	CurrentGuard->CurrentRoom->LeaveRoom(CurrentGuard);
	ABBRoomBase* NewGuardRoom = CurrentGuard->CurrentLevel->GetRoomAt(CurrentGuardPatrolTarget);
	NewGuardRoom->EnterRoom(CurrentGuard, true, true);
	NewGuardRoom->TeleportToRoom(CurrentGuard);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteBrownOut(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	for (int32 Index = 0; Index < GameMode->GetCurrentLevel()->AmountFloors; Index++)
	{
		TArray<ABBRoomBase*> RoomsOfFloor = Character->CurrentLevel->GetRoomsOfFloor(Index);
		for (ABBRoomBase* Room : RoomsOfFloor)
		{
			if (Room->bHasAlarm)
			{
				GameMode->GetGuard(Index)->DrawNextPatrolTarget();
				Room->SetAlarm(false);
			}
		}
	}
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteChangeOfPlans(class ABBCharacterBase* Character)
{
	TArray<ABBRoomBase*> RoomsOfFloor = Character->CurrentLevel->GetRoomsOfFloor(Character);
	for (ABBRoomBase* Room : RoomsOfFloor)
	{
		if (Room->bHasAlarm)
		{
			EndEventCheck(Character);
			return;
		}
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	ABBGuard* CurrentGuard = GameMode->GetCurrentGuardPawn();
	CurrentGuard->DrawNextPatrolTarget();
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteVideoLoop(class ABBCharacterBase* Character)
{
	UBBEventComponent* Component = NewObject<UBBEventComponent>(Character, UBBEventComponent::StaticClass(), FName(*GETENUMSTRING("EEventType", EEventType::VideoLoop)));
	Component->RegisterComponent();
	Component->InitEvent(EEventType::VideoLoop);
	EndEventCheck(Character);
}

FDelegateHandle UBBEventLibrary::HeadsUpDelegateHandle;

void UBBEventLibrary::ExecuteHeadsUp(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	HeadsUpDelegateHandle = GameMode->OnTurnStart_NonDynamic.AddStatic(&UBBEventLibrary::GiveHeadsUpAction);
	EndEventCheck(Character);
}

void UBBEventLibrary::GiveHeadsUpAction(class ABBPlayerCharacterBase* Character)
{
	Character->Actions.SetCurrentActions(Character->Actions.GetCurrentActions() + 1);
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->OnTurnStart_NonDynamic.Remove(HeadsUpDelegateHandle);
}

void UBBEventLibrary::ExecuteSqueak(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	ABBGuard* CurrentGuard = GameMode->GetCurrentGuardPawn();
	if (CurrentGuard->CurrentRoom->CharactersInRoom.Num() > 1)
	{
		// cant get closer to player in same room
		EndEventCheck(Character);
		return;
	}
	TArray<ABBPlayerCharacterBase*> PlayersOnSameFloor = Character->CurrentLevel->GetPlayersOnFloor(Character->CurrentRoom->Position.Floor);
	TArray<TArray<FRoomPosition>> AllPathsToPlayers;
	for (ABBPlayerCharacterBase* Player : PlayersOnSameFloor)
	{
		FRoomPosition PlayerPosition = Player->CurrentRoom->Position;
		AllPathsToPlayers.Add(CurrentGuard->GetPath(PlayerPosition));
	}
	int32 ShortestPathIndex = 0;
	if (AllPathsToPlayers.Num() > 1)
	{
		for (int32 Index = 1; Index < AllPathsToPlayers.Num(); Index++)
		{
			if (AllPathsToPlayers[Index].Num() < AllPathsToPlayers[ShortestPathIndex].Num())
			{
				ShortestPathIndex = Index;
			}
		}
	}
	CurrentGuard->GetLevelNavigationComponent()->MoveToRoomPosition(AllPathsToPlayers[ShortestPathIndex][0], false, true);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteThrowVoice(class ABBCharacterBase* Character)
{
	TArray<ABBRoomBase*> RoomsOfFloor = Character->CurrentLevel->GetRoomsOfFloor(Character);
	for (ABBRoomBase* Room : RoomsOfFloor)
	{
		if (Room->bHasAlarm)
		{
			EndEventCheck(Character);
			return;
		}
	}
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->SetValidRoomsToChoose(GetValidThrowVoiceRooms(Character));
	GameMode->OnRoomChosen_NonDynamic.AddStatic(&UBBEventLibrary::OnThrowVoiceRoomChosen);
	GameMode->EnterChooseRoom();
}

TArray<class ABBRoomBase*> UBBEventLibrary::GetValidThrowVoiceRooms(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	ABBGuard* GuardOfFloor = GameMode->GetGuard(Character->CurrentRoom->Position.Floor);
	FRoomPosition CurrentGuardDestinationPosition = GuardOfFloor->CurrentPatrolInfo.TargetRoom;
	ABBRoomBase* DestinationRoom = GameMode->GetCurrentLevel()->GetRoomAt(CurrentGuardDestinationPosition);
	TArray<ABBRoomBase*> ValidRooms;

	FRoomPosition NorthPosition(CurrentGuardDestinationPosition.PosX, CurrentGuardDestinationPosition.PosY + 1, CurrentGuardDestinationPosition.Floor);
	ABBRoomBase* NorthRoom = Character->CurrentLevel->GetRoomAt(NorthPosition);
	if (NorthRoom && !DestinationRoom->Walls[EDirection::North])
	{
		ValidRooms.Add(NorthRoom);
	}

	FRoomPosition EastPosition(CurrentGuardDestinationPosition.PosX + 1, CurrentGuardDestinationPosition.PosY, CurrentGuardDestinationPosition.Floor);
	ABBRoomBase* EastRoom = Character->CurrentLevel->GetRoomAt(EastPosition);
	if (EastRoom && !DestinationRoom->Walls[EDirection::East])
	{
		ValidRooms.Add(EastRoom);
	}

	FRoomPosition SouthPosition(CurrentGuardDestinationPosition.PosX, CurrentGuardDestinationPosition.PosY - 1, CurrentGuardDestinationPosition.Floor);
	ABBRoomBase* SouthRoom = Character->CurrentLevel->GetRoomAt(SouthPosition);
	if (SouthRoom && !DestinationRoom->Walls[EDirection::South])
	{
		ValidRooms.Add(SouthRoom);
	}

	FRoomPosition WestPosition(CurrentGuardDestinationPosition.PosX - 1, CurrentGuardDestinationPosition.PosY, CurrentGuardDestinationPosition.Floor);
	ABBRoomBase* WestRoom = Character->CurrentLevel->GetRoomAt(WestPosition);
	if (WestRoom && !DestinationRoom->Walls[EDirection::West])
	{
		ValidRooms.Add(WestRoom);
	}
	return ValidRooms;
}

void UBBEventLibrary::OnThrowVoiceRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom)
{
	if (ChosenRoom)
	{
		ABBGuard* GuardOfFloor = GameMode->GetGuard(ChosenRoom->Position.Floor);
		GuardOfFloor->SetNewPatrolTarget(ChosenRoom->Position);
	}
	GameMode->OnRoomChosen_NonDynamic.Clear();
	EndEventCheck(GameMode);
}

void UBBEventLibrary::ExecuteJumpTheGun(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	if (GameMode->GetAllPlayers().Num() == 1)
	{
		EndEventCheck(Character);
		return;
	}
	GameMode->SetNextPlayerIndex(GameMode->GetNextPlayerIndex());
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteGoWithYourGut(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	TArray<class ABBRoomBase *> ValidGoWithYourGutRooms = GetValidGoWithYourGutRooms(Character);
	GameMode->SetValidRoomsToChoose(ValidGoWithYourGutRooms, ValidGoWithYourGutRooms.Num() == 0);
	GameMode->OnRoomChosen_NonDynamic.AddStatic(&UBBEventLibrary::OnGoWithYourGutRoomChosen);
	GameMode->EnterChooseRoom();
}

TArray<class ABBRoomBase*> UBBEventLibrary::GetValidGoWithYourGutRooms(class ABBCharacterBase* Character)
{
	ABBRoomBase* CurrentRoom = Character->CurrentRoom;
	FRoomPosition CurrentPosition = CurrentRoom->Position;
	TArray<ABBRoomBase*> ValidRooms;

	FRoomPosition NorthPosition(CurrentPosition.PosX, CurrentPosition.PosY + 1, CurrentPosition.Floor);
	ABBRoomBase* NorthRoom = Character->CurrentLevel->GetRoomAt(NorthPosition);
	if (NorthRoom && !CurrentRoom->Walls[EDirection::North] && !NorthRoom->bIsDiscovered)
	{
		ValidRooms.Add(NorthRoom);
	}

	FRoomPosition EastPosition(CurrentPosition.PosX + 1, CurrentPosition.PosY, CurrentPosition.Floor);
	ABBRoomBase* EastRoom = Character->CurrentLevel->GetRoomAt(EastPosition);
	if (EastRoom && !CurrentRoom->Walls[EDirection::East] && !EastRoom->bIsDiscovered)
	{
		ValidRooms.Add(EastRoom);
	}

	FRoomPosition SouthPosition(CurrentPosition.PosX, CurrentPosition.PosY - 1, CurrentPosition.Floor);
	ABBRoomBase* SouthRoom = Character->CurrentLevel->GetRoomAt(SouthPosition);
	if (SouthRoom && !CurrentRoom->Walls[EDirection::South] && !SouthRoom->bIsDiscovered)
	{
		ValidRooms.Add(SouthRoom);
	}

	FRoomPosition WestPosition(CurrentPosition.PosX - 1, CurrentPosition.PosY, CurrentPosition.Floor);
	ABBRoomBase* WestRoom = Character->CurrentLevel->GetRoomAt(WestPosition);
	if (WestRoom && !CurrentRoom->Walls[EDirection::West] && !WestRoom->bIsDiscovered)
	{
		ValidRooms.Add(WestRoom);
	}
	return ValidRooms;
}

void UBBEventLibrary::OnGoWithYourGutRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom)
{
	if (ChosenRoom)
		GameMode->GetCurrentPlayerCharacter()->GetLevelNavigationComponent()->MoveToRoom(ChosenRoom, false, true);
	GameMode->OnRoomChosen_NonDynamic.Clear();
	EndEventCheck(GameMode);
}

void UBBEventLibrary::ExecuteShiftChange(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->bShiftChangeActive = true;
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteTimeLock(class ABBCharacterBase* Character)
{
	UBBEventComponent* Component = NewObject<UBBEventComponent>(Character, UBBEventComponent::StaticClass(), FName(*GETENUMSTRING("EEventType", EEventType::TimeLock)));
	Component->RegisterComponent();
	Component->InitEvent(EEventType::TimeLock);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecuteGymnastics(class ABBCharacterBase* Character)
{
	UBBEventComponent* Component = NewObject<UBBEventComponent>(Character, UBBEventComponent::StaticClass(), FName(*GETENUMSTRING("EEventType", EEventType::Gymnastics)));
	Component->RegisterComponent();
	Component->InitEvent(EEventType::Gymnastics);
	EndEventCheck(Character);
}

void UBBEventLibrary::ExecutePeekhole(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->SetValidRoomsToChoose(GetValidPeekholeRooms(Character));
	GameMode->OnRoomChosen_NonDynamic.AddStatic(&UBBEventLibrary::OnPeekholeRoomChosen);
	GameMode->EnterChooseRoom();
}

TArray<class ABBRoomBase*> UBBEventLibrary::GetValidPeekholeRooms(class ABBCharacterBase* Character)
{
	TArray<ABBRoomBase*> RoomsOfFloor = Character->CurrentLevel->GetRoomsOfFloor(Character);
	TArray<ABBRoomBase*> ValidRooms;
	for (ABBRoomBase* Room : RoomsOfFloor)
	{
		if (Room->Position.IsNextToRoom(Character->CurrentRoom->Position))
			ValidRooms.Add(Room);
	}
	if (Character->CurrentRoom->bHasStairsUp && Character->CurrentRoom->Position.Floor < Character->CurrentLevel->AmountFloors - 1)
	{
		ValidRooms.Add(Character->CurrentLevel->GetRoomAt(Character->CurrentRoom->Position.GetAboveRoomPosition()));
	}
	if (Character->CurrentRoom->bHasStairsDown && Character->CurrentRoom->Position.Floor > 0)
	{
		ValidRooms.Add(Character->CurrentLevel->GetRoomAt(Character->CurrentRoom->Position.GetBelowRoomPosition()));
	}
	return ValidRooms;
}

void UBBEventLibrary::OnPeekholeRoomChosen(class ABBMainGameMode* GameMode, class ABBRoomBase* ChosenRoom)
{
	if (ChosenRoom)
		ChosenRoom->SetDiscovered();
	GameMode->OnRoomChosen_NonDynamic.Clear();
	EndEventCheck(GameMode);
}

void UBBEventLibrary::ExecuteBuddySystem(class ABBCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->OnPlayerChosen_NonDynamic.AddStatic(&UBBEventLibrary::OnBuddySystemCharacterChosen);
	GameMode->bCanCancelChoosePlayer = false;
	GameMode->EnterChoosePlayer();
}

void UBBEventLibrary::OnBuddySystemCharacterChosen(class ABBPlayerCharacterBase* Character)
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(Character);
	GameMode->OnPlayerChosen_NonDynamic.Clear();
	ABBRoomBase* CurrentRoom = GameMode->GetCurrentPlayerCharacter()->CurrentRoom;
	Character->CurrentRoom->LeaveRoom(Character);
	CurrentRoom->EnterRoom(Character, true, true);
	CurrentRoom->TeleportToRoom(Character);
	EndEventCheck(GameMode);
}

void UBBEventLibrary::EndEventCheck(UObject* WorldContext)
{
	UBBUtilities::GetMainGameMode(WorldContext)->LeaveEventCheck();
}
