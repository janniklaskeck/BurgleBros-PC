// Fill out your copyright notice in the Description page of Project Settings.

#include "BBPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "BBRoomBase.h"
#include "BBPlayerCharacterBase.h"
#include "BBAIController.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "BBLevel.h"
#include "BBLevelNavigationComponent.h"
#include "BBToolInfo.h"
#include "BBPlayerCameraManager.h"
#include "BBComputerComponent.h"

ABBPlayerController::ABBPlayerController()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ABBPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);
}

void ABBPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckHovered();
}

void ABBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("ClickRoom", IE_Released, this, &ABBPlayerController::ClickRoom);
	InputComponent->BindAction("DeselectRoom", IE_Released, this, &ABBPlayerController::DeselectRoom);

	InputComponent->BindAction("MoveUpFloor", IE_Released, this, &ABBPlayerController::MoveUpFloor);
	InputComponent->BindAction("MoveDownFloor", IE_Released, this, &ABBPlayerController::MoveDownFloor);

	InputComponent->BindAction("Pause", IE_Pressed, this, &ABBPlayerController::PauseGame).bExecuteWhenPaused = true;

	InputComponent->BindAxis("Zoom", this, &ABBPlayerController::CameraZoom);
	InputComponent->BindAxis("MoveForward", this, &ABBPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABBPlayerController::MoveRight);

	InputComponent->BindAxis("MouseForward", this, &ABBPlayerController::MouseForward);
	InputComponent->BindAxis("MouseRight", this, &ABBPlayerController::MouseRight);
}

void ABBPlayerController::CheckHovered()
{
	FHitResult HitResult;
	GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
	ABBRoomBase* HitRoom = Cast<ABBRoomBase>(HitResult.Actor);
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	GameMode->SetHoveredRoom(HitRoom);
}

void ABBPlayerController::ClickRoom()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(GetWorld());
	GameMode->SelectHoveredRoom();
}

void ABBPlayerController::DeselectRoom()
{
	UBBUtilities::GetMainGameMode(GetWorld())->DeselectRoom();
}

void ABBPlayerController::MoveUpFloor()
{
	ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
	CameraManager->MoveUpFloor();
}

void ABBPlayerController::MoveDownFloor()
{
	ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
	CameraManager->MoveDownFloor();
}

void ABBPlayerController::PauseGame()
{
	ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
	GameMode->SetGamePaused(!GameMode->IsGamePaused());
}

void ABBPlayerController::CameraZoom(float Delta)
{
	ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
	CameraManager->Zoom(Delta);
}

void ABBPlayerController::MoveForward(float Delta)
{
	ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
	CameraManager->MoveForward(Delta * UBBUtilities::GetBBGameInstance(this)->GetCameraSpeedScale());
}

void ABBPlayerController::MoveRight(float Delta)
{
	ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
	CameraManager->MoveRight(Delta * UBBUtilities::GetBBGameInstance(this)->GetCameraSpeedScale());
}

void ABBPlayerController::MouseForward(float Delta)
{
	float MouseX;
	float MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		int32 Width;
		int32 Height;
		GetViewportSize(Width, Height);
		if (MouseY >= 0.f && MouseY <= 15.f)
		{
			ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
			CameraManager->MoveForward(UBBUtilities::GetBBGameInstance(this)->GetCameraSpeedScale());
		}
		else if (MouseY >= Height - 15.f && MouseY <= Height)
		{
			ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
			CameraManager->MoveForward(-UBBUtilities::GetBBGameInstance(this)->GetCameraSpeedScale());
		}
	}
}

void ABBPlayerController::MouseRight(float Delta)
{
	float MouseX;
	float MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		int32 Width;
		int32 Height;
		GetViewportSize(Width, Height);
		if (MouseX >= 0.f && MouseX <= 15.f)
		{
			ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
			CameraManager->MoveRight(-UBBUtilities::GetBBGameInstance(this)->GetCameraSpeedScale());
		}
		else if (MouseX >= Width - 15.f && MouseX <= Width)
		{
			ABBPlayerCameraManager* CameraManager = UBBUtilities::GetPlayerCameraManager(this, 0);
			CameraManager->MoveRight(UBBUtilities::GetBBGameInstance(this)->GetCameraSpeedScale());
		}
	}
}

ABBCharacterBase* ABBPlayerController::GetControlledCharacter()
{
	if (!ControlledCharacterAIController)
		return nullptr;
	return Cast<ABBCharacterBase>(ControlledCharacterAIController->GetPawn());
}

void ABBPlayerController::GetTool(TArray<EToolType> Type)
{
	UBBUtilities::GetMainGameMode(this)->GetCurrentPlayerCharacter()->Tools.Append(Type);
}

void ABBPlayerController::AddLoot(TArray<ELootType> Types)
{
	for (ELootType Type : Types)
		UBBUtilities::GetMainGameMode(this)->GetCurrentPlayerCharacter()->AddLootComponent(Type);
}

void ABBPlayerController::AddComputerTokens(int32 Amount)
{
	TArray<ABBRoomBase *> ComputerRooms;
	ABBLevel* Level = UBBUtilities::GetMainGameMode(this)->GetCurrentLevel();
	ComputerRooms.Append(Level->GetRoomsOfType(ERoomType::ComputerFingerprint));
	ComputerRooms.Append(Level->GetRoomsOfType(ERoomType::ComputerLaser));
	ComputerRooms.Append(Level->GetRoomsOfType(ERoomType::ComputerMotion));
	for (ABBRoomBase* Room : ComputerRooms)
	{
		Room->GetRoomFunctionAs<UBBComputerComponent>()->AddTokens(Amount);
	}
}

void ABBPlayerController::ResetActions()
{
	UBBUtilities::GetMainGameMode(this)->GetCurrentPlayerCharacter()->Actions.Reset();
}

void ABBPlayerController::ResetStealth()
{
	UBBUtilities::GetMainGameMode(this)->GetCurrentPlayerCharacter()->AddStealthToken();
}
