// Fill out your copyright notice in the Description page of Project Settings.

#include "BBMainGameMode.h"
#include "BBPlayerCharacterBase.h"
#include "BBGuard.h"
#include "SlateWrapperTypes.h" // BIND_UOBJECT_DELEGATE macro
#include "BBAIController.h"
#include "BBPlayerController.h"
#include "BBUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "BBLevel.h"
#include "BBEventLibrary.h"
#include "burglebros.h"
#include "BBToolLibrary.h"
#include "BBLootLibrary.h"
#include "BBGameInstance.h"
#include "BBSplineDrawer.h"
#include "BBLevelNavigationComponent.h"

DEFINE_LOG_CATEGORY(LogMainGameMode);

void ABBMainGameMode::BeginPlay()
{
	Super::BeginPlay();
	UBBLootLibrary::InitLootDeck();
	UBBToolLibrary::InitDeck();
	UBBEventLibrary::InitEventDeck();
}

void ABBMainGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (SMGame)
	{
		SMGame->Update(DeltaSeconds);
	}
}

bool ABBMainGameMode::IsGamePaused() const
{
	return UGameplayStatics::IsGamePaused(this);
}

void ABBMainGameMode::SetGamePaused(bool IsPaused)
{
	UGameplayStatics::SetGamePaused(this, IsPaused);
	if (IsPaused)
	{
		if (!PauseWidget && PauseWidgetClass)
		{
			PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);
			PauseWidget->AddToViewport();
		}
		else
		{
			PauseWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (PauseWidget)
			PauseWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

ABBAIController* ABBMainGameMode::GetGuardController(int32 FloorIndex)
{
	return Cast<ABBAIController>(GuardControllers[FloorIndex]);
}

ABBAIController* ABBMainGameMode::GetCurrentGuardController()
{
	int32 CurrentFloorIndex = GetCurrentPlayerCharacter()->CurrentRoom->Position.Floor;
	return GetGuardController(CurrentFloorIndex);
}

ABBGuard* ABBMainGameMode::GetCurrentGuardPawn()
{
	return Cast<ABBGuard>(GetCurrentGuardController()->GetPawn());
}

TArray<ABBGuard*> ABBMainGameMode::GetAllGuards()
{
	TArray<ABBGuard*> AllGuards;
	for (ABBAIController* GuardController : GuardControllers)
	{
		AllGuards.Add(Cast<ABBGuard>(GuardController->GetPawn()));
	}
	return AllGuards;
}

ABBGuard* ABBMainGameMode::GetGuard(int32 Floor)
{
	if (Floor < 0 || Floor >= GuardControllers.Num())
	{
		return nullptr;
	}
	return Cast<ABBGuard>(GuardControllers[Floor]->GetPawn());
}

int32 ABBMainGameMode::GetNextPlayerIndex()
{
	if (CurrentPlayerIndex + 1 < Players.Num())
	{
		return CurrentPlayerIndex + 1;
	}
	else
	{
		return 0;
	}
}

void ABBMainGameMode::SetNextPlayerIndex(int32 Index)
{
	if (Index >= 0 && Index < Players.Num())
	{
		CurrentPlayerIndex = Index;
	}
	else
	{
		CurrentPlayerIndex = 0;
	}
}

TArray<ABBPlayerCharacterBase*> ABBMainGameMode::GetAllPlayers()
{
	return TArray<ABBPlayerCharacterBase*>(Players);
}

void ABBMainGameMode::SetCurrentLevel(ABBLevel* NewLevel)
{
	CurrentLevel = NewLevel;
	SetupStateMachine();
	SplineDrawer = GetWorld()->SpawnActor<ABBSplineDrawer>(SplineDrawerClass, FVector::ZeroVector, FRotator::ZeroRotator);
}

ABBLevel* ABBMainGameMode::GetCurrentLevel() const
{
	return CurrentLevel;
}

bool ABBMainGameMode::IsPlayerTurn()
{
	return SMGame->GetCurrentState() == EBBMainGameStates::PlayerTurn;
}

bool ABBMainGameMode::IsEventTurn()
{
	return SMGame->GetCurrentState() == EBBMainGameStates::CheckForEvent;
}

bool ABBMainGameMode::IsGuardTurn()
{
	return SMGame->GetCurrentState() == EBBMainGameStates::GuardTurn;
}

class ABBRoomBase* ABBMainGameMode::GetSelectedRoom() const
{
	return SelectedRoom;
}

void ABBMainGameMode::SelectHoveredRoom()
{
	DeselectRoom();
	if (!GetHoveredRoom() || !CanSelectRoom())
		return;

	SelectedRoom = GetHoveredRoom();
	SelectedRoom->SetSelected(true);
	SelectedRoom->SetHovered(false);
	OnRoomSelectionChange.Broadcast(SelectedRoom);
	SplineDrawer->DrawPlayerPath();
}

bool ABBMainGameMode::CanSelectRoom()
{
	return IsPlayerTurn() || (SMGame->GetCurrentState() == EBBMainGameStates::GameAction && GameActionEnterEvent == EBBMainGameEvents::ChooseRoomEnter);
}

bool ABBMainGameMode::CanHoverRoom()
{
	return CanSelectRoom();
}

void ABBMainGameMode::DeselectRoom()
{
	if (SelectedRoom)
	{
		SelectedRoom->SetSelected(false);
		SelectedRoom->SetHovered(false);
		SelectedRoom = nullptr;
	}
	SplineDrawer->ClearPlayerPath();
	OnRoomSelectionChange.Broadcast(SelectedRoom);
}

class ABBRoomBase* ABBMainGameMode::GetHoveredRoom() const
{
	return HoveredRoom;
}

void ABBMainGameMode::SetHoveredRoom(class ABBRoomBase* Room)
{
	if (Room == HoveredRoom || !CanHoverRoom())
		return;
	if (HoveredRoom)
		HoveredRoom->SetHovered(false);

	HoveredRoom = Room;
	if (HoveredRoom)
		HoveredRoom->SetHovered(true);
	if (!SelectedRoom)
		OnRoomHoverChange.Broadcast(HoveredRoom);
}

void ABBMainGameMode::UpdateSelectedRoom()
{
	OnRoomSelectionChange.Broadcast(SelectedRoom);
}

ABBPlayerCharacterBase* ABBMainGameMode::GetCurrentPlayerCharacter()
{
	if (Players.Num() == 0 || CurrentPlayerIndex >= Players.Num())
	{
		return nullptr;
	}
	return Cast<ABBPlayerCharacterBase>(Players[CurrentPlayerIndex]);
}

class ABBPlayerCharacterBase* ABBMainGameMode::GetPlayerWithType(EPlayerClass PlayerClass)
{
	for (ABBPlayerCharacterBase* Player : Players)
	{
		if (Player->PlayerType == PlayerClass)
			return Player;
	}
	return nullptr;
}

int32 ABBMainGameMode::GetCurrentPlayerIndex()
{
	return CurrentPlayerIndex;
}

ABBPlayerCharacterBase* ABBMainGameMode::SpawnNextPlayer(int32 Index)
{
	if (Index >= PlayersToSpawn.Num() || PlayersToSpawn.Num() == 0)
		return nullptr;
	FPlayerClass PlayerClassData = PlayersToSpawn[Index];
	UClass* PlayerClass = PlayerClassInfoList->GetPlayerClassInfo(PlayerClassData.PlayerClass)->Class;
	ABBPlayerCharacterBase* Character = GetWorld()->SpawnActor<ABBPlayerCharacterBase>(PlayerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	Character->InitPlayerClass(UBBUtilities::GetActualPlayerClass(PlayerClassData.PlayerClass, PlayerClassData.bIsAlternativeClass));
	Character->InitCharacter(GetCurrentLevel(), PlayerStartingRoom->Position);
	ABBAIController* AIController = GetWorld()->SpawnActor<ABBAIController>(FVector::ZeroVector, FRotator::ZeroRotator);
	AIController->Possess(Character);
	ABBPlayerController* PlayerController = UBBUtilities::GetBBPlayerController(this);
	PlayerController->SetControlledCharacter(AIController);
	Players.Add(Character);
	OnCharactersSpawned.Broadcast();

	return Character;
}

ABBGuard* ABBMainGameMode::SpawnGuard(int32 Floor, int32 InitialMaxActions, bool DrawFirstPatrolTarget /*= true*/)
{
	UWorld* World = GetWorld();
	ABBGuard* Guard = World->SpawnActor<ABBGuard>(GuardClass, FVector::ZeroVector, FRotator::ZeroRotator);
	Guard->InitCharacter(GetCurrentLevel(), CurrentLevel->GetRandomRoomPositionOnFloor(Floor));
	Guard->Actions.SetMaxActions(InitialMaxActions);
	ABBAIController* GuardAIController = World->SpawnActor<ABBAIController>(FVector::ZeroVector, FRotator::ZeroRotator);
	GuardAIController->Possess(Guard);
	GuardControllers.Add(GuardAIController);
	if (DrawFirstPatrolTarget)
	{
		Guard->DrawFirstPatrolTarget();
	}
	return Guard;
}

bool ABBMainGameMode::HasPlayerClass(EPlayerClass PlayerClass)
{
	for (ABBCharacterBase* Player : Players)
	{
		if (Cast<ABBPlayerCharacterBase>(Player)->PlayerType == PlayerClass)
			return true;
	}
	return false;
}

void ABBMainGameMode::OnFirstRoomChosen(class ABBRoomBase* ChosenRoom)
{
	PlayerStartingRoom = ChosenRoom;
	OnRoomChosen.RemoveDynamic(this, &ABBMainGameMode::OnFirstRoomChosen);
}

TArray<class ABBRoomBase*> ABBMainGameMode::GetValidFirstRooms()
{
	TArray<ABBRoomBase*> FirstFloorRooms = CurrentLevel->GetRoomsOfFloor(0);
	return FirstFloorRooms.FilterByPredicate([this](ABBRoomBase* Room)
	{
		bool XOnSide = Room->Position.PosX == 0 || Room->Position.PosX == CurrentLevel->FloorWidth - 1;
		bool YOnSide = Room->Position.PosY == 0 || Room->Position.PosY == CurrentLevel->FloorWidth - 1;
		return XOnSide || YOnSide;
	});
}

void ABBMainGameMode::SetupStateMachine()
{
	SMGame.Reset(new BBMainGameStateMachine(EBBMainGameStates::Initial, EBBMainGameEvents::InitialDummy));
	SMGame->SetLogStringifier(
		MakeShareable(new TBBEnumStringifier<EBBMainGameStates>(TO_STRING(EBBMainGameStates))),
		MakeShareable(new TBBEnumStringifier<EBBMainGameEvents>(TO_STRING(EBBMainGameEvents)))
	);
	SMGame->State(EBBMainGameStates::Initial)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ChooseRoomEnter)
		.IgnoreNotSetEvents()
		.OnEnter(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnEnterStateDelegate, OnEnterInitial));

	SMGame->State(EBBMainGameStates::PlayerTurn)
		.Transition(EBBMainGameStates::CheckForEvent, EBBMainGameEvents::PlayerFinished)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ChooseRoomEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::CharacterActionEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ChooseToolEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ToolEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::DrawCardEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::RoomActionEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::RollDiceEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::PreventAlarmEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ChoosePlayerEnter)
		.Transition(EBBMainGameStates::GameOver, EBBMainGameEvents::GameWon)
		.Transition(EBBMainGameStates::GameOver, EBBMainGameEvents::GameLost)
		.OnEnter(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnEnterStateDelegate, OnEnterPlayerTurn))
		.OnUpdate(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnUpdateState, OnUpdatePlayerTurn))
		.OnLeave(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnLeaveStateDelegate, OnLeavePlayerTurn));

	SMGame->State(EBBMainGameStates::GameAction)
		.Transition(EBBMainGameStates::PlayerTurn, EBBMainGameEvents::GameActionLeaveToPlayer)
		.Transition(EBBMainGameStates::GuardTurn, EBBMainGameEvents::GameActionLeaveToGuard)
		.Transition(EBBMainGameStates::CheckForEvent, EBBMainGameEvents::GameActionLeaveToEvent)
		.Transition(EBBMainGameStates::GameOver, EBBMainGameEvents::GameActionLeaveToGameOver)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ChooseRoomEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::DrawCardEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ToolEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::PreventAlarmEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::RoomActionEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::RollDiceEnter)
		.OnEnter(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnEnterStateDelegate, OnEnterGameAction))
		.OnLeave(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnLeaveStateDelegate, OnLeaveGameAction));

	SMGame->State(EBBMainGameStates::CheckForEvent)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ChooseRoomEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::DrawCardEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::ChoosePlayerEnter)
		.Transition(EBBMainGameStates::GameAction, EBBMainGameEvents::RollDiceEnter)
		.Transition(EBBMainGameStates::GuardTurn, EBBMainGameEvents::EventFinished)
		.Transition(EBBMainGameStates::GameOver, EBBMainGameEvents::GameLost)
		.OnEnter(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnEnterStateDelegate, OnEnterEventCheck))
		.OnUpdate(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnUpdateState, OnUpdateEventCheck))
		.OnLeave(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnLeaveStateDelegate, OnLeaveEventCheck));

	SMGame->State(EBBMainGameStates::GuardTurn)
		.Transition(EBBMainGameStates::PlayerTurn, EBBMainGameEvents::GuardFinished)
		.Transition(EBBMainGameStates::GameOver, EBBMainGameEvents::GameLost)
		.OnEnter(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnEnterStateDelegate, OnEnterGuardTurn))
		.OnLeave(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnLeaveStateDelegate, OnLeaveGuardTurn));

	SMGame->State(EBBMainGameStates::GameOver)
		.OnEnter(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnEnterStateDelegate, OnEnterGameOver))
		.OnLeave(BIND_UOBJECT_DELEGATE(BBMainGameStateMachine::OnLeaveStateDelegate, OnLeaveGameOver));
}

void ABBMainGameMode::OnEnterInitial(EBBMainGameStates PrevState, EBBMainGameEvents Event)
{
	CurrentLevel->BuildLevel();
	GetWorld()->SpawnActor<AActor>(RoofPlatformClass, RoofLocation, FRotator::ZeroRotator);

	SpawnGuard(0, 2, false);

	for (FPlayerClass PlayerClass : UBBUtilities::GetBBGameInstance(this)->GetSelectedPlayerClasses())
	{
		if (PlayerClass.bIsEnabled)
		{
			PlayersToSpawn.Add(PlayerClass);
		}
	}

	SetValidRoomsToChoose(GetValidFirstRooms(), false);
	OnRoomChosen.AddDynamic(this, &ABBMainGameMode::OnFirstRoomChosen);
	SMGame->NotifyLatent(EBBMainGameEvents::ChooseRoomEnter);
	OnInitialization.Broadcast();
}

void ABBMainGameMode::OnEnterPlayerTurn(EBBMainGameStates PrevState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("EnterPlayer"));
	bool IsInitialTurn = false;
	if (Players.Num() == 0)
	{
		SpawnNextPlayer(0);
		GetGuard(0)->DrawFirstPatrolTarget();
		IsInitialTurn = true;
	}
	if (PrevState == EBBMainGameStates::GuardTurn)
	{
		UBBGameInstance* GameInstance = UBBUtilities::GetBBGameInstance(this);
		if (Players.Num() < PlayersToSpawn.Num())
		{
			SpawnNextPlayer(Players.Num());
		}
		for (int32 Index = 0; Index < Players.Num(); Index++)
		{
			++CurrentPlayerIndex;
			if (CurrentPlayerIndex > Players.Num() - 1)
				CurrentPlayerIndex = 0;
			if (!Players[CurrentPlayerIndex]->bIsEscaped)
			{
				break;
			}
		}

	}
	if (IsInitialTurn || PrevState == EBBMainGameStates::GuardTurn)
	{
		GetCurrentPlayerCharacter()->Actions.Reset();
		GetCurrentPlayerCharacter()->StartTurn();
		OnCharacterChanged.Broadcast(CurrentPlayerIndex);
		OnTurnStart.Broadcast(GetCurrentPlayerCharacter());
		OnTurnStart_NonDynamic.Broadcast(GetCurrentPlayerCharacter());
		DOnEnterPlayerTurn.Broadcast();
	}
	SplineDrawer->ClearPlayerPath();
}

void ABBMainGameMode::OnUpdatePlayerTurn(float DeltaTime)
{
	if (bWantsToLeavePlayerTurn)
	{
		if (!IsAnyPlayerMoving())
		{
			SMGame->NotifyLatent(EBBMainGameEvents::PlayerFinished);
			bWantsToLeavePlayerTurn = false;
		}
	}
}

void ABBMainGameMode::OnLeavePlayerTurn(EBBMainGameStates NextState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("LeavePlayer"));
	if (NextState == EBBMainGameStates::CheckForEvent)
	{
		SplineDrawer->ClearPlayerPath();
		OnTurnEnd.Broadcast();
		GetCurrentPlayerCharacter()->FinishTurn();
	}
	DOnLeavePlayerTurn.Broadcast();
}

void ABBMainGameMode::OnEnterGameAction(EBBMainGameStates PrevState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("EnterGameAction: (PreviousState: %s)(EnterEvent: %s)"), *GETENUMSTRING("EBBMainGameStates", PrevState), *GETENUMSTRING("EBBMainGameEvents", Event));
	if (PrevState != EBBMainGameStates::GameAction)
		StateBeforeGameAction = PrevState;
	GameActionEnterEvent = Event;

	switch (Event)
	{
	case EBBMainGameEvents::ChooseRoomEnter:
		for (ABBRoomBase* Room : ValidRoomsForChooseRoom)
		{
			Room->SetRenderCustomDepth(true);
		}
		DOnEnterChooseRoom.Broadcast();
		break;
	case EBBMainGameEvents::CharacterActionEnter:
		DOnEnterCharacterAction.Broadcast();
		break;
	case EBBMainGameEvents::ChooseToolEnter:
		DOnEnterChooseTool.Broadcast();
		break;
	case EBBMainGameEvents::ToolEnter:
		DOnEnterTool.Broadcast();
		break;
	case EBBMainGameEvents::RoomActionEnter:
		if (!RoomActionRoom)
			RoomActionRoom = GetCurrentPlayerCharacter()->CurrentRoom;
		DOnEnterRoomAction.Broadcast();
		break;
	case EBBMainGameEvents::DrawCardEnter:
		DOnEnterDrawCard.Broadcast();
		break;
	case EBBMainGameEvents::RollDiceEnter:
		DOnEnterRollDice.Broadcast();
		break;
	case EBBMainGameEvents::PreventAlarmEnter:
		DOnEnterPreventAlarm.Broadcast();
		break;
	case EBBMainGameEvents::ChoosePlayerEnter:
		DOnEnterChoosePlayer.Broadcast();
		break;
	}
}

void ABBMainGameMode::OnLeaveGameAction(EBBMainGameStates NextState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("LeaveGameAction: (NextState: %s)(EnterEvent: %s)"), *GETENUMSTRING("EBBMainGameStates", NextState), *GETENUMSTRING("EBBMainGameEvents", Event));
	if (GameActionEnterEvent == EBBMainGameEvents::ChooseRoomEnter)
	{
		for (ABBRoomBase* Room : ValidRoomsForChooseRoom)
		{
			Room->SetRenderCustomDepth(false);
		}
	}
	RoomActionRoom = nullptr;
	DOnLeaveGameAction.Broadcast();
}

void ABBMainGameMode::OnEnterEventCheck(EBBMainGameStates PrevState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("EnterEvent"));
	DeselectRoom();
	SplineDrawer->ClearPlayerPath();
	if (GetCurrentPlayerCharacter()->Actions.IsPastEventThreshold())
	{
		DOnEnterEventCheck.Broadcast();
	}
	else
	{
		UE_LOG(LogMainGameMode, Warning, TEXT("SkipEvent"));
		LeaveEventCheck();
	}
}

void ABBMainGameMode::OnUpdateEventCheck(float DeltaTime)
{
	if (bWantsToLeaveEventCheck)
	{
		if (!IsAnyoneMoving())
		{
			SMGame->NotifyLatent(EBBMainGameEvents::EventFinished);
			bWantsToLeaveEventCheck = false;
		}
	}
}

void ABBMainGameMode::OnLeaveEventCheck(EBBMainGameStates NextState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("LeaveEvent"));
	DOnLeaveEventCheck.Broadcast();
}

void ABBMainGameMode::OnEnterGuardTurn(EBBMainGameStates PrevState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("EnterGuard"));
	TArray<ABBGuard*> GuardsToMove;
	if (bShiftChangeActive)
	{
		for (int32 Index = 0; Index < CurrentLevel->AmountFloors; Index++)
		{
			if (Index != GetCurrentGuardPawn()->CurrentRoom->Position.Floor && GetGuard(Index))
				GuardsToMove.Add(GetGuard(Index));
		}
		bShiftChangeActive = false;
	}
	else
	{
		GuardsToMove.Add(GetCurrentGuardPawn());
	}
	if (GuardsToMove.Num() == 0)
	{
		LeaveGuardTurn();
	}
	else
	{
		for (ABBGuard* Guard : GuardsToMove)
		{
			if (Guard->bHasDonuts)
			{
				Guard->bHasDonuts = false;
				if (GuardsToMove.Num() == 1)
				{
					SMGame->NotifyLatent(EBBMainGameEvents::GuardFinished);
				}
				continue;
			}
			if (Guard->CurrentRoom->bContainsRavenAlt && Guard->GetClosestRoomWithAlarm().Floor == -1)
			{
				Guard->CurrentRoom->bContainsRavenAlt = false;
				SMGame->NotifyLatent(EBBMainGameEvents::GuardFinished);
				continue;
			}
			Guard->StartTurn();
		}
	}
	DOnEnterGuardTurn.Broadcast();
}

void ABBMainGameMode::OnLeaveGuardTurn(EBBMainGameStates NextState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("LeaveGuard"));
	for (int32 Index = 0; Index < CurrentLevel->AmountFloors; Index++)
	{
		if (GetGuard(Index))
		{
			GetGuard(Index)->Actions.Reset();
		}
	}
	DOnLeaveGuardTurn.Broadcast();
}

void ABBMainGameMode::OnEnterGameOver(EBBMainGameStates PrevState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("EnterGameOver"));
	GameOverEnterEvent = Event;
	DOnEnterGameOver.Broadcast();
}

void ABBMainGameMode::OnLeaveGameOver(EBBMainGameStates NextState, EBBMainGameEvents Event)
{
	UE_LOG(LogMainGameMode, Warning, TEXT("LeaveGameOver"));
	DOnLeaveGameOver.Broadcast();
}

//////////////////////////////////////////////////////////////////////////


void ABBMainGameMode::SetValidRoomsToChoose(TArray<ABBRoomBase*> ValidRooms, bool CanCancel /*= true*/)
{
	ValidRoomsForChooseRoom.Reset(48);
	ValidRoomsForChooseRoom.Append(ValidRooms);
	bCanCancelChooseRoom = CanCancel;
}

bool ABBMainGameMode::IsSelectedRoomValid()
{
	return SelectedRoom && ValidRoomsForChooseRoom.Contains(SelectedRoom);
}

void ABBMainGameMode::TeleportToRoof(class ABBPlayerCharacterBase* Player)
{
	Player->CurrentRoom->LeaveRoom(Player);
	Player->bIsEscaped = true;
	Player->SetActorLocation(RoofLocation + FVector(0.f, 0.f, 100.0f));
}

void ABBMainGameMode::LeavePlayerTurn()
{
	if (SMGame->GetCurrentState() == EBBMainGameStates::PlayerTurn)
	{
		bWantsToLeavePlayerTurn = true;
	}
}

bool ABBMainGameMode::IsAnyPlayerMoving()
{
	for (ABBPlayerCharacterBase* Player : Players)
	{
		if (Player->GetLevelNavigationComponent()->IsMoving())
		{
			return true;
		}
	}
	return false;
}

void ABBMainGameMode::LeaveEventCheck()
{
	if (SMGame->GetCurrentState() == EBBMainGameStates::CheckForEvent)
		bWantsToLeaveEventCheck = true;
}

bool ABBMainGameMode::IsAnyoneMoving()
{
	bool AnyPlayerMoving = IsAnyPlayerMoving();
	if (AnyPlayerMoving)
		return true;
	for (int32 Index = 0; Index < CurrentLevel->AmountFloors; Index++)
	{
		if (GetGuard(Index) && GetGuard(Index)->GetLevelNavigationComponent()->IsMoving())
		{
			return true;
		}
	}
	return false;
}

void ABBMainGameMode::LeaveGuardTurn()
{
	if (SMGame->GetCurrentState() == EBBMainGameStates::GuardTurn)
	{
		GetCurrentGuardPawn()->OnGuardMovementEnd.Clear();
		SMGame->NotifyLatent(EBBMainGameEvents::GuardFinished);
	}
}

void ABBMainGameMode::EnterChooseRoom()
{
	SMGame->NotifyLatent(EBBMainGameEvents::ChooseRoomEnter);
}

void ABBMainGameMode::EnterRoomAction()
{
	SMGame->NotifyLatent(EBBMainGameEvents::RoomActionEnter);
}

void ABBMainGameMode::EnterChooseTool()
{
	SMGame->NotifyLatent(EBBMainGameEvents::ChooseToolEnter);
}

void ABBMainGameMode::EnterTool()
{
	SMGame->NotifyLatent(EBBMainGameEvents::ToolEnter);
}

void ABBMainGameMode::EnterCharacterAction()
{
	SMGame->NotifyLatent(EBBMainGameEvents::CharacterActionEnter);
}

void ABBMainGameMode::EnterDrawCard()
{
	SMGame->NotifyLatent(EBBMainGameEvents::DrawCardEnter);
}

void ABBMainGameMode::EnterRollDice()
{
	SMGame->NotifyLatent(EBBMainGameEvents::RollDiceEnter);
}

void ABBMainGameMode::EnterPreventAlarm()
{
	SMGame->NotifyLatent(EBBMainGameEvents::PreventAlarmEnter);
}

void ABBMainGameMode::EnterChoosePlayer()
{
	SMGame->NotifyLatent(EBBMainGameEvents::ChoosePlayerEnter);
}

void ABBMainGameMode::LeaveGameAction()
{
	if (GameActionEnterEvent == EBBMainGameEvents::ChooseRoomEnter)
	{
		OnRoomChosen.Broadcast(SelectedRoom);
		OnRoomChosen_NonDynamic.Broadcast(this, SelectedRoom);
	}
	else if (GameActionEnterEvent == EBBMainGameEvents::ChoosePlayerEnter)
	{
		OnPlayerChosen.Broadcast(SelectedCharacter);
		OnPlayerChosen_NonDynamic.Broadcast(SelectedCharacter);
		bCanCancelChoosePlayer = true;
	}

	if (StateBeforeGameAction == EBBMainGameStates::PlayerTurn || StateBeforeGameAction == EBBMainGameStates::Initial)
	{
		SMGame->NotifyLatent(EBBMainGameEvents::GameActionLeaveToPlayer);
	}
	else if (StateBeforeGameAction == EBBMainGameStates::GuardTurn)
	{
		SMGame->NotifyLatent(EBBMainGameEvents::GameActionLeaveToGuard);
	}
	else if (StateBeforeGameAction == EBBMainGameStates::CheckForEvent)
	{
		bool WasChoosingRoom = GameActionEnterEvent == EBBMainGameEvents::ChooseRoomEnter;
		bool DrawnGoWithYourGut = UBBEventLibrary::GetLastEvent() == EEventType::GoWithYourGut;
		if (!(DrawnGoWithYourGut && WasChoosingRoom && SelectedRoom && SelectedRoom->RoomType == ERoomType::Keypad))
		{
			SMGame->NotifyLatent(EBBMainGameEvents::GameActionLeaveToGuard);
		}
	}
	else
	{
		// TODO
		SMGame->NotifyLatent(EBBMainGameEvents::GameActionLeaveToGameOver);
	}
}

void ABBMainGameMode::WinGame()
{
	SMGame->NotifyLatent(EBBMainGameEvents::GameWon);
}

void ABBMainGameMode::LoseGame()
{
	SMGame->NotifyLatent(EBBMainGameEvents::GameLost);
}
