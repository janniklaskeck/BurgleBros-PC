// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "BBStateMachine.h"
#include "BBPlayerClass.h"
#include "BBPlayerClassInfoList.h"
#include "BBDelegates.h"
#include "BBGameInstance.h"
#include "BBMainGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMainGameMode, Log, All);

UENUM(BlueprintType)
enum EBBMainGameStates
{
	Initial,
	PlayerTurn,
	GameAction,
	CheckForEvent,
	GuardTurn,
	GameOver
};

UENUM(BlueprintType)
enum EBBMainGameEvents
{
	InitialDummy,
	PlayerFinished,
	ChooseRoomEnter,
	RoomActionEnter,
	ChooseToolEnter,
	ToolEnter,
	CharacterActionEnter,
	DrawCardEnter,
	RollDiceEnter,
	PreventAlarmEnter,
	ChoosePlayerEnter,
	GameActionLeaveToPlayer,
	GameActionLeaveToEvent,
	GameActionLeaveToGuard,
	GameActionLeaveToGameOver,
	EventFinished,
	GuardFinished,
	GameWon,
	GameLost
};

typedef TBBStateMachine<EBBMainGameStates, EBBMainGameEvents> BBMainGameStateMachine;

class ABBCharacterBase;
class ABBGuard;
class ABBAIController;

UCLASS(BlueprintType)
class BURGLEBROS_API ABBMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	TUniquePtr<BBMainGameStateMachine> SMGame;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsGamePaused() const;
	UFUNCTION(BlueprintCallable)
		void SetGamePaused(bool bIsPaused);
	UPROPERTY(BlueprintAssignable)
		FOnBoolChange GamePaused;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> PauseWidgetClass;
	UPROPERTY()
		class UUserWidget* PauseWidget;

	ABBAIController* GetGuardController(int32 FloorIndex);
	ABBAIController* GetCurrentGuardController();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		ABBGuard* GetCurrentGuardPawn();
	TArray<ABBGuard*> GetAllGuards();
	UFUNCTION(BlueprintCallable)
		ABBGuard* GetGuard(int32 Floor);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		ABBPlayerCharacterBase* GetCurrentPlayerCharacter();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		class ABBPlayerCharacterBase* GetPlayerWithType(EPlayerClass PlayerClass);
	int32 GetCurrentPlayerIndex();
	int32 GetNextPlayerIndex();
	void SetNextPlayerIndex(int32 Index);
	TArray<ABBPlayerCharacterBase*> GetAllPlayers();

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class ABBSplineDrawer> SplineDrawerClass;
	class ABBSplineDrawer* SplineDrawer;

	void SetCurrentLevel(class ABBLevel* NewLevel);
	class ABBLevel* GetCurrentLevel() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UBBPlayerClassInfoList* PlayerClassInfoList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<ABBGuard> GuardClass;

	UPROPERTY(BlueprintAssignable)
		FOnAction OnCharactersSpawned;
	UPROPERTY(BlueprintAssignable)
		FOnIntChange OnCharacterChanged;
	UPROPERTY(BlueprintAssignable)
		FPlayerCharacterAction OnTurnStart;
	FOnCharacterAction_NonDynamic OnTurnStart_NonDynamic;
	UPROPERTY(BlueprintAssignable)
		FOnAction OnTurnEnd;

	UFUNCTION(BlueprintCallable)
		bool IsPlayerTurn();
	UFUNCTION(BlueprintCallable)
		bool IsEventTurn();
	UFUNCTION(BlueprintCallable)
		bool IsGuardTurn();

	class ABBRoomBase* GetSelectedRoom() const;
	void SelectHoveredRoom();
	bool CanSelectRoom();
	bool CanHoverRoom();
	UFUNCTION(BlueprintCallable)
		void DeselectRoom();
	class ABBRoomBase* GetHoveredRoom() const;
	void SetHoveredRoom(class ABBRoomBase* Room);
	void UpdateSelectedRoom();
	UPROPERTY(BlueprintAssignable)
		FOnRoomAction OnRoomSelectionChange;
	UPROPERTY(BlueprintAssignable)
		FOnRoomAction OnRoomHoverChange;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class ABBRoomBase* SelectedRoom;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class ABBRoomBase* HoveredRoom;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		ABBLevel* CurrentLevel;

	bool IsInitialPlayer = true;
	int32 CurrentPlayerIndex = 0;

	class ABBRoomBase* PlayerStartingRoom;
	TArray<FPlayerClass> PlayersToSpawn;
	ABBPlayerCharacterBase* SpawnNextPlayer(int32 Index);
public:
	ABBGuard* SpawnGuard(int32 Floor, int32 InitialMaxActions, bool DrawFirstPatrolTarget = true);
private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		TArray<ABBPlayerCharacterBase*> Players;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		TArray<ABBAIController*> GuardControllers;
public:
	bool bShiftChangeActive = false;
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool HasPlayerClass(EPlayerClass PlayerClass);

private:
	//////////////////////////////////////////////////////////////////////////
	// STATE MACHINE
	void SetupStateMachine();

	void OnEnterInitial(EBBMainGameStates PrevState, EBBMainGameEvents Event);

	void OnEnterPlayerTurn(EBBMainGameStates PrevState, EBBMainGameEvents Event);
	void OnUpdatePlayerTurn(float DeltaTime);
	void OnLeavePlayerTurn(EBBMainGameStates NextState, EBBMainGameEvents Event);

	void OnEnterChooseRoomAction(EBBMainGameStates PrevState, EBBMainGameEvents Event);
	void OnLeaveChooseRoomAction(EBBMainGameStates NextState, EBBMainGameEvents Event);

	void OnEnterGameAction(EBBMainGameStates PrevState, EBBMainGameEvents Event);
	void OnLeaveGameAction(EBBMainGameStates NextState, EBBMainGameEvents Event);

	void OnEnterEventCheck(EBBMainGameStates PrevState, EBBMainGameEvents Event);
	void OnUpdateEventCheck(float DeltaTime);
	void OnLeaveEventCheck(EBBMainGameStates NextState, EBBMainGameEvents Event);

	void OnEnterGuardTurn(EBBMainGameStates PrevState, EBBMainGameEvents Event);
	void OnLeaveGuardTurn(EBBMainGameStates NextState, EBBMainGameEvents Event);

	void OnEnterGameOver(EBBMainGameStates PrevState, EBBMainGameEvents Event);
	void OnLeaveGameOver(EBBMainGameStates NextState, EBBMainGameEvents Event);

	EBBMainGameStates StateBeforeChooseRoom;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<class ABBRoomBase*> ValidRoomsForChooseRoom;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bCanCancelChooseRoom = true;
	UFUNCTION()
		void OnFirstRoomChosen(class ABBRoomBase* ChosenRoom);
	TArray<class ABBRoomBase*> GetValidFirstRooms();

public:
	UPROPERTY(BlueprintReadOnly)
		class ABBRoomBase* RoomActionRoom = nullptr;
	UPROPERTY(BlueprintReadOnly)
		bool bCanCancelChoosePlayer = true;
	EBBMainGameStates StateBeforeGameAction;
	EBBMainGameEvents GameActionEnterEvent;
	UPROPERTY(BlueprintReadOnly)
		class ABBRoomBase* PreventAlarmRoom;
	UPROPERTY(BlueprintReadWrite)
		class ABBPlayerCharacterBase* SelectedCharacter;
	UPROPERTY(BlueprintReadOnly)
		int32 RollDiceAmount;
	UPROPERTY(BlueprintReadWrite)
		TArray<int32> RollDiceResults;
	void SetValidRoomsToChoose(TArray<class ABBRoomBase*> ValidRooms, bool CanCancel = true);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsSelectedRoomValid();

	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EBBMainGameEvents> GameOverEnterEvent;
private:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> RoofPlatformClass;
	UPROPERTY(EditDefaultsOnly)
		FVector RoofLocation;
public:
	void TeleportToRoof(class ABBPlayerCharacterBase* Player);
public:
	UFUNCTION(BlueprintCallable)
		void LeavePlayerTurn();
	bool bWantsToLeavePlayerTurn = false;
	bool IsAnyPlayerMoving();
	UFUNCTION()
		void LeaveEventCheck();
	bool bWantsToLeaveEventCheck = false;
	bool IsAnyoneMoving();
	UFUNCTION()
		void LeaveGuardTurn();

	UFUNCTION(BlueprintCallable)
		void EnterChooseRoom();
	UFUNCTION(BlueprintCallable)
		void EnterRoomAction();
	UFUNCTION(BlueprintCallable)
		void EnterChooseTool();
	UFUNCTION(BlueprintCallable)
		void EnterTool();
	UFUNCTION(BlueprintCallable)
		void EnterCharacterAction();
	UFUNCTION(BlueprintCallable)
		void EnterDrawCard();
	UFUNCTION(BlueprintCallable)
		void EnterRollDice();
	UFUNCTION(BlueprintCallable)
		void EnterPreventAlarm();
	UFUNCTION(BlueprintCallable)
		void EnterChoosePlayer();
	UFUNCTION(BlueprintCallable)
		void LeaveGameAction();
	void WinGame();
	void LoseGame();
public:
	UPROPERTY(BlueprintAssignable)
		FOnAction OnInitialization;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterPlayerTurn;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnLeavePlayerTurn;

	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterChooseRoom;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterDrawCard;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterRollDice;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterCharacterAction;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterChooseTool;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterTool;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterRoomAction;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterPreventAlarm;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterChoosePlayer;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnLeaveGameAction;

	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterEventCheck;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnLeaveEventCheck;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterGuardTurn;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnLeaveGuardTurn;

	UPROPERTY(BlueprintAssignable)
		FOnAction DOnEnterGameOver;
	UPROPERTY(BlueprintAssignable)
		FOnAction DOnLeaveGameOver;

	UPROPERTY(BlueprintAssignable)
		FOnRoomAction OnRoomChosen;
	FOnRoomAction_NonDynamic OnRoomChosen_NonDynamic;
	UPROPERTY(BlueprintAssignable)
		FPlayerCharacterAction OnPlayerChosen;
	FOnCharacterAction_NonDynamic OnPlayerChosen_NonDynamic;
};