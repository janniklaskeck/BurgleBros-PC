// Fill out your copyright notice in the Description page of Project Settings.

#include "BBRoomBase.h"
#include "BBGuard.h"
#include "BBPlayerCharacterBase.h"
#include "BBLevel.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BBLootLibrary.h"
#include "BBToolLibrary.h"
#include "BBPlayerCameraManager.h"
#include "BBRoomInfoList.h"
#include "Components/CapsuleComponent.h"

TArray<int32> ABBRoomBase::AmountAlarms;

ABBRoomBase::ABBRoomBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (Walls.Num() == 0)
	{
		Walls.Add(EDirection::North, false);
		Walls.Add(EDirection::East, false);
		Walls.Add(EDirection::South, false);
		Walls.Add(EDirection::West, false);
	}

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRootComponent;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>("FloorMesh");
	FloorMesh->SetupAttachment(RootComponent);

	NorthWall = CreateDefaultSubobject<UStaticMeshComponent>("NorthWall");
	NorthWall->SetupAttachment(RootComponent);
	EastWall = CreateDefaultSubobject<UStaticMeshComponent>("EastWall");
	EastWall->SetupAttachment(RootComponent);
	SouthWall = CreateDefaultSubobject<UStaticMeshComponent>("SouthWall");
	SouthWall->SetupAttachment(RootComponent);
	WestWall = CreateDefaultSubobject<UStaticMeshComponent>("WestWall");
	WestWall->SetupAttachment(RootComponent);

	SelectionMesh = CreateDefaultSubobject<UStaticMeshComponent>("SelectionMesh");
	SelectionMesh->SetupAttachment(RootComponent);

	Player1Position = CreateDefaultSubobject<USceneComponent>("Player1Position");
	Player1Position->SetupAttachment(RootComponent);
	Player2Position = CreateDefaultSubobject<USceneComponent>("Player2Position");
	Player2Position->SetupAttachment(RootComponent);
	Player3Position = CreateDefaultSubobject<USceneComponent>("Player3Position");
	Player3Position->SetupAttachment(RootComponent);
	Player4Position = CreateDefaultSubobject<USceneComponent>("Player4Position");
	Player4Position->SetupAttachment(RootComponent);
	GuardPosition = CreateDefaultSubobject<USceneComponent>("GuardPosition");
	GuardPosition->SetupAttachment(RootComponent);

	RoomTypeWidget = CreateDefaultSubobject<UWidgetComponent>("RoomTypeWidget");
	RoomTypeWidget->SetupAttachment(RootComponent);

	SetWall(EDirection::North, Walls[EDirection::North]);
	SetWall(EDirection::East, Walls[EDirection::East]);
	SetWall(EDirection::South, Walls[EDirection::South]);
	SetWall(EDirection::West, Walls[EDirection::West]);
	FloorMesh->SetStaticMesh(DoorMesh);

	ItemMarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMarkerMesh");
	ItemMarkerMesh->SetupAttachment(RootComponent);
	ItemMarkerWidget = CreateDefaultSubobject<UWidgetComponent>("ItemMarkerWidget");
	ItemMarkerWidget->SetupAttachment(ItemMarkerMesh);

	SetPosition(Position);
}

// Called when the game starts or when spawned
void ABBRoomBase::BeginPlay()
{
	Super::BeginPlay();
	RoomFunction = NewObject<UBBRoomFunctionComponent>(this, RoomFunctionClass, "RoomFunctionComponent");
	RoomFunction->RegisterComponent();
	UMaterialInterface* Material = NorthWall->GetMaterial(0);
	NorthWallMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	NorthWall->SetMaterial(0, NorthWallMaterialInstance);
	Material = EastWall->GetMaterial(0);
	EastWallMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	EastWall->SetMaterial(0, EastWallMaterialInstance);
	Material = SouthWall->GetMaterial(0);
	SouthWallMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	SouthWall->SetMaterial(0, SouthWallMaterialInstance);
	Material = WestWall->GetMaterial(0);
	WestWallMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	WestWall->SetMaterial(0, WestWallMaterialInstance);

	Material = SelectionMesh->GetMaterial(0);
	SelectionMeshMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	SelectionMesh->SetMaterial(0, SelectionMeshMaterialInstance);

	if (RoomType != ERoomType::Safe)
	{
		SafeCombinationNumber = FMath::RandRange(1, 6);
	}
	if (RoomType == ERoomType::Stairs)
	{
		bHasStairsUp = true;
	}
	else if (RoomType == ERoomType::Walkway)
	{
		bHasStairsDown = true;
	}
	else if (RoomType == ERoomType::Lavatory)
	{
		AmountStealthTokens = 3;
	}
	if (bStartDiscovered)
		SetDiscovered();
}

void ABBRoomBase::SetRenderCustomDepth(bool IsEnabled)
{
	FloorMesh->SetRenderCustomDepth(IsEnabled);
	NorthWall->SetRenderCustomDepth(IsEnabled);
	EastWall->SetRenderCustomDepth(IsEnabled);
	SouthWall->SetRenderCustomDepth(IsEnabled);
	WestWall->SetRenderCustomDepth(IsEnabled);
}

void ABBRoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ABBPlayerCameraManager* CameraManger = UBBUtilities::GetPlayerCameraManager(this, 0);
	if (ItemWidgetVisible() && CameraManger)
	{
		if (!ItemMarkerMesh->IsVisible())
		{
			ItemMarkerMesh->SetVisibility(true);
		}
		FRotator CameraRotation = CameraManger->GetCameraRotation();
		ItemMarkerWidget->SetWorldRotation((-CameraRotation.Vector()).Rotation());
	}
	else
	{
		if (ItemMarkerMesh->IsVisible())
		{
			ItemMarkerMesh->SetVisibility(false);
		}
	}
}

void ABBRoomBase::SetWall(EDirection Direction, bool IsWall)
{
	Walls[Direction] = IsWall;
	switch (Direction)
	{
	case EDirection::North:
		NorthWall->SetStaticMesh(IsWall ? WallMesh : DoorMesh);
		break;
	case EDirection::East:
		EastWall->SetStaticMesh(IsWall ? WallMesh : DoorMesh);
		break;
	case EDirection::South:
		SouthWall->SetStaticMesh(IsWall ? WallMesh : DoorMesh);
		break;
	case EDirection::West:
		WestWall->SetStaticMesh(IsWall ? WallMesh : DoorMesh);
		break;
	}
	UpdateWalls();
}

void ABBRoomBase::SetWallColors()
{
	FLinearColor RoomDiscoveredColor;
	if (RoomInfoList)
		RoomDiscoveredColor = RoomInfoList->GetRoomByType(RoomType)->DiscoveredColor;
	if (NorthWallMaterialInstance)
		NorthWallMaterialInstance->SetVectorParameterValue("WallColor", bIsDiscovered ? RoomDiscoveredColor : FLinearColor(1, 1, 1, 1));
	if (EastWallMaterialInstance)
		EastWallMaterialInstance->SetVectorParameterValue("WallColor", bIsDiscovered ? RoomDiscoveredColor : FLinearColor(1, 1, 1, 1));
	if (SouthWallMaterialInstance)
		SouthWallMaterialInstance->SetVectorParameterValue("WallColor", bIsDiscovered ? RoomDiscoveredColor : FLinearColor(1, 1, 1, 1));
	if (WestWallMaterialInstance)
		WestWallMaterialInstance->SetVectorParameterValue("WallColor", bIsDiscovered ? RoomDiscoveredColor : FLinearColor(1, 1, 1, 1));
}

void ABBRoomBase::SetPosition(int32 PosX, int32 PosY, int32 Floor)
{
	SetPosition(FRoomPosition(PosX, PosY, Floor));
}

void ABBRoomBase::SetPosition(FRoomPosition RoomPosition)
{
	Position = RoomPosition;
	FText PositionText = FText::FromString(FString::Printf(TEXT("%d | %d"), Position.PosX, Position.PosY));
}

void ABBRoomBase::UpdateWalls()
{
	SetWallColors();
}

void ABBRoomBase::SetHidden(bool IsHidden)
{
	SetActorHiddenInGame(IsHidden);
	ECollisionResponse Response = IsHidden ? ECollisionResponse::ECR_Ignore : ECollisionResponse::ECR_Block;
	FloorMesh->SetCollisionResponseToChannel(ECC_Visibility, Response);
	NorthWall->SetCollisionResponseToChannel(ECC_Visibility, Response);
	EastWall->SetCollisionResponseToChannel(ECC_Visibility, Response);
	SouthWall->SetCollisionResponseToChannel(ECC_Visibility, Response);
	WestWall->SetCollisionResponseToChannel(ECC_Visibility, Response);

	for (ABBCharacterBase* Character : CharactersInRoom)
	{
		Character->SetCharacterHidden(IsHidden);
		Character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, Response);
	}
}

bool ABBRoomBase::IsSameFloor(ABBRoomBase* Other)
{
	if (!Other)
		return false;
	return Position.Floor == Other->Position.Floor;
}

FVector ABBRoomBase::GetFreePlayerPosition()
{
	TArray<ABBCharacterBase*> PlayerCharacters = CharactersInRoom.FilterByPredicate([](ABBCharacterBase* Character)
	{
		return Cast<ABBPlayerCharacterBase>(Character);
	});
	int32 PlayersPresent = PlayerCharacters.Num();
	switch (PlayersPresent)
	{
	case 0:
		return Player1Position->GetComponentLocation();
	case 1:
		return Player2Position->GetComponentLocation();
	case 2:
		return Player3Position->GetComponentLocation();
	default:
		return Player4Position->GetComponentLocation();
	}
}

FVector ABBRoomBase::GetGuardPosition()
{
	return GuardPosition->GetComponentLocation();
}

void ABBRoomBase::TeleportToRoom(class ABBCharacterBase* Character, bool FocusCameraOnCharacter /*= true*/)
{
	FVector Offset(0.f, 0.f, 100.f);
	if (Cast<ABBGuard>(Character))
	{
		Character->SetActorLocation(GetGuardPosition() + Offset);
	}
	else
	{
		Character->SetActorLocation(GetFreePlayerPosition() + Offset);
		if (FocusCameraOnCharacter)
			UBBUtilities::GetPlayerCameraManager(this, 0)->FocusOnViewTarget();
	}
}

bool ABBRoomBase::ContainsCharacter(ABBCharacterBase* Character)
{
	return CharactersInRoom.Contains(Character);
}

bool ABBRoomBase::ContainsGuard()
{
	if (CharactersInRoom.Num() == 0)
		return false;
	for (ABBCharacterBase* Character : CharactersInRoom)
	{
		if (Cast<ABBGuard>(Character))
		{
			return true;
		}
	}
	return false;
}

bool ABBRoomBase::EnterRoom(ABBCharacterBase* Character, bool IgnoreEffects, bool IgnoreCosts)
{
	bool IsNotBlocked = OnRoomEntered(Character, IgnoreEffects, IgnoreCosts);
	CharactersInRoom.AddUnique(Character);
	if (Cast<ABBGuard>(Character))
	{
		return true;
	}
	return IsNotBlocked;
}

void ABBRoomBase::LeaveRoom(ABBCharacterBase* Character)
{
	OnRoomLeft(Character);
	CharactersInRoom.Remove(Character);
}

bool ABBRoomBase::CanEnterRoom(ABBCharacterBase* Character)
{
	return RoomFunction->CanEnter(Character);
}

void ABBRoomBase::SetDiscovered()
{
	if (!bIsDiscovered)
	{
		bIsDiscovered = true;
		SetWallColors();
		ABBMainGameMode* GameMode = UBBUtilities::GetMainGameMode(this);
		if (GameMode)
			GameMode->UpdateSelectedRoom();
	}
}

UBBRoomFunctionComponent* ABBRoomBase::GetRoomFunction() const
{
	return RoomFunction;
}

class UBBRoomInfo* ABBRoomBase::GetRoomInfo() const
{
	if (RoomInfoList)
	{
		return RoomInfoList->GetRoomByType(RoomType);
	}
	return nullptr;
}

bool ABBRoomBase::ItemWidgetVisible()
{
	return bHasAlarm
		|| (bIsDiscovered && (bHasStairsUp || bHasStairsDown))
		|| bContainsRaven
		|| bContainsRavenAlt
		|| (bIsDiscovered && AmountStealthTokens > 0)
		|| ToolsInRoom.Num() > 0
		|| LootInRoom.Num() > 0;
}

bool ABBRoomBase::OnRoomEntered(ABBCharacterBase* Character, bool IgnoreEffects, bool IgnoreCosts)
{
	bool IsNotBlocked = RoomFunction->OnEnter(Character, IgnoreEffects, IgnoreCosts);
	if (Cast<ABBGuard>(Character))
	{
		OnRoomCharacterEnter.Broadcast(Character);
		return true;
	}
	else
	{
		SetDiscovered();
		if (!CurrentLevel->GetFloor(Position.Floor).bIsRevealed)
		{
			CurrentLevel->GetFloor(Position.Floor).RevealFloor();
		}
		if (IsNotBlocked)
		{
			ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
			if (LootInRoom.Num() > 0)
			{
				for (int32 Index = LootInRoom.Num() - 1; Index >= 0; Index--)
				{
					ELootType Loot = LootInRoom[Index];
					if (!(Loot == ELootType::GoldBar && PlayerCharacter->Loot.Contains(ELootType::GoldBar)))
					{
						RemoveLootFromRoom(Loot);
						UBBLootLibrary::AttachLootToPlayer(Loot, PlayerCharacter);
					}
				}
			}
			if (ToolsInRoom.Num() > 0)
			{
				for (int32 Index = ToolsInRoom.Num() - 1; Index >= 0; Index--)
				{
					EToolType Tool = ToolsInRoom[Index];
					PlayerCharacter->Tools.Add(Tool);
					ToolsInRoom.RemoveAt(Index);
				}
			}
		}
	}
	OnRoomCharacterEnter.Broadcast(Character);
	return IsNotBlocked;
}

void ABBRoomBase::OnRoomLeft(ABBCharacterBase* Character)
{
	RoomFunction->OnLeave(Character);
	OnRoomCharacterLeave.Broadcast(Character);
}

void ABBRoomBase::SetAlarm(bool HasAlarm)
{
	if (HasAlarm && UBBToolLibrary::ActiveTools.Contains(EToolType::EMP))
	{
		return;
	}
	bool AlarmChanged = bHasAlarm != HasAlarm;
	bHasAlarm = HasAlarm;
	if (!AlarmChanged)
		return;
	if (HasAlarm)
	{
		++AmountAlarms[Position.Floor];
	}
	else
	{
		--AmountAlarms[Position.Floor];
	}
	OnAlarmChanged.Broadcast(bHasAlarm);
	OnRoomItemChanged.Broadcast();
}

void ABBRoomBase::SetSelected(bool IsSelected)
{
	bIsSelected = IsSelected;
	if (bIsSelected)
	{
		SetHovered(false);
	}
	SelectionMesh->SetVisibility(IsSelected);
	SelectionMeshMaterialInstance->SetVectorParameterValue("Color", SelectedColor);
}

void ABBRoomBase::SetHovered(bool IsHovered)
{
	if (bIsSelected)
		return;
	bIsHovered = IsHovered;
	SelectionMesh->SetVisibility(bIsHovered);
	SelectionMeshMaterialInstance->SetVectorParameterValue("Color", HoveredColor);
}

void ABBRoomBase::SetHasStairsDown(bool HasStairs)
{
	bHasStairsDown = HasStairs;
}

void ABBRoomBase::SetHasStairsUp(bool HasStairs)
{
	bHasStairsUp = HasStairs;
}

void ABBRoomBase::MoveDown(class ABBPlayerCharacterBase* Character)
{
	ABBRoomBase* RoomBelow = CurrentLevel->GetRoomAt(Position.GetBelowRoomPosition());
	if (!RoomBelow || !Character)
		return;
	LeaveRoom(Character);
	RoomBelow->EnterRoom(Character);
	RoomBelow->TeleportToRoom(Character);
}

ERoomType ABBRoomBase::GetRoomType()
{
	return RoomType;
}

bool ABBRoomBase::HasAlarm()
{
	return bHasAlarm;
}

bool ABBRoomBase::IsDiscovered()
{
	return bIsDiscovered;
}

bool ABBRoomBase::HasStairsUp()
{
	return bHasStairsUp;
}

bool ABBRoomBase::HasStairsDown()
{
	return bHasStairsDown;
}

bool ABBRoomBase::IsSafeCombinationNumberCracked()
{
	return bSafeCombinationNumberCracked;
}

bool ABBRoomBase::ContainsRaven(bool IsAltRaven)
{
	if (IsAltRaven)
		return bContainsRavenAlt;
	return bContainsRaven;
}

int32 ABBRoomBase::GetSafeCombinationNumber()
{
	return SafeCombinationNumber;
}

TArray<ELootType> ABBRoomBase::GetLootInRoom()
{
	return TArray<ELootType>(LootInRoom);
}

TArray<EToolType> ABBRoomBase::GetToolsInRoom()
{
	return TArray<EToolType>(ToolsInRoom);
}

bool ABBRoomBase::IsAlarmRoom()
{
	return RoomType == ERoomType::Camera
		|| RoomType == ERoomType::Detector
		|| RoomType == ERoomType::Laser
		|| RoomType == ERoomType::Motion
		|| RoomType == ERoomType::Thermo
		|| RoomType == ERoomType::Fingerprint;
}

void ABBRoomBase::AddLootToRoom(ELootType Loot)
{
	LootInRoom.Add(Loot);
	OnRoomItemChanged.Broadcast();
}

void ABBRoomBase::AddToolToRoom(EToolType Tool)
{
	ToolsInRoom.Add(Tool);
	OnRoomItemChanged.Broadcast();
}

void ABBRoomBase::RemoveLootFromRoom(ELootType Loot)
{
	LootInRoom.RemoveSingle(Loot);
	OnRoomItemChanged.Broadcast();
}

void ABBRoomBase::RemoveToolToRoom(EToolType Tool)
{
	ToolsInRoom.Remove(Tool);
	OnRoomItemChanged.Broadcast();
}

void ABBRoomBase::SetRavenInRoom(bool HasRaven, bool IsAltRaven)
{
	if (IsAltRaven)
	{
		bContainsRavenAlt = HasRaven;
	}
	else
	{
		bContainsRaven = HasRaven;
	}
	OnRoomItemChanged.Broadcast();
}
