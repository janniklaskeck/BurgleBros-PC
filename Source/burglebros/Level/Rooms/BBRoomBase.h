// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "BBRoomPosition.h"
#include "BBRoomFunctionComponent.h"
#include "BBLootInfo.h"
#include "BBToolInfo.h"
#include "BBDelegates.h"
#include "BBRoomBase.generated.h"

class ABBCharacterBase;

UCLASS(Blueprintable)
class BURGLEBROS_API ABBRoomBase : public AActor
{
	GENERATED_BODY()
public:
	ABBRoomBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Category = Walls, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SceneRootComponent;
	UPROPERTY(Category = Walls, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* NorthWall;
	UPROPERTY(Category = Walls, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* EastWall;
	UPROPERTY(Category = Walls, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* SouthWall;
	UPROPERTY(Category = Walls, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* WestWall;
	UPROPERTY(Category = Walls, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* FloorMesh;
	UPROPERTY(Category = Walls, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* SelectionMesh;

	UPROPERTY(Category = Characters, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* Player1Position;
	UPROPERTY(Category = Characters, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* Player2Position;
	UPROPERTY(Category = Characters, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* Player3Position;
	UPROPERTY(Category = Characters, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* Player4Position;
	UPROPERTY(Category = Characters, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* GuardPosition;

	UPROPERTY(Category = Items, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ItemMarkerMesh;
	UPROPERTY(Category = Items, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* ItemMarkerWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* RoomTypeWidget;

	UPROPERTY(Category = Room, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UBBRoomFunctionComponent* RoomFunction;

public:
	void SetRenderCustomDepth(bool IsEnabled);
protected:

	UPROPERTY()
		UMaterialInstanceDynamic* NorthWallMaterialInstance;
	UPROPERTY()
		UMaterialInstanceDynamic* EastWallMaterialInstance;
	UPROPERTY()
		UMaterialInstanceDynamic* SouthWallMaterialInstance;
	UPROPERTY()
		UMaterialInstanceDynamic* WestWallMaterialInstance;
	UPROPERTY()
		UMaterialInstanceDynamic* SelectionMeshMaterialInstance;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		FLinearColor SelectedColor;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		FLinearColor HoveredColor;
public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere) // Currently used for Main Menu
		bool bStartDiscovered = false;
	bool bIsDiscovered = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FRoomPosition Position;
	UPROPERTY(EditDefaultsOnly)
		ERoomType RoomType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UBBRoomFunctionComponent> RoomFunctionClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<EDirection, bool> Walls;
	UFUNCTION(BlueprintCallable)
		void SetWall(EDirection Direction, bool IsWall);
	void SetWallColors();

	void SetPosition(int32 PosX, int32 PosY, int32 Floor);
	void SetPosition(FRoomPosition RoomPosition);

	void UpdateWalls();

	void SetHidden(bool IsHidden);

	bool IsSameFloor(ABBRoomBase* Other);
public:
	FVector GetFreePlayerPosition();
	FVector GetGuardPosition();
	void TeleportToRoom(class ABBCharacterBase* Character, bool FocusCameraOnCharacter = true);

public:
	TArray<ABBCharacterBase*> CharactersInRoom;
	bool ContainsCharacter(ABBCharacterBase* Character);
	bool ContainsGuard();

	UPROPERTY(BlueprintAssignable)
		FCharacterAction OnRoomCharacterEnter;
	UPROPERTY(BlueprintAssignable)
		FCharacterAction OnRoomCharacterLeave;

	// Returns if the Character is blocked by this room or not
	bool EnterRoom(ABBCharacterBase* Character, bool IgnoreEffects = false, bool IgnoreCosts = false);
	void LeaveRoom(ABBCharacterBase* Character);
	bool OnRoomEntered(ABBCharacterBase* Character, bool IgnoreEffects, bool IgnoreCosts);
	void OnRoomLeft(ABBCharacterBase* Character);
	bool CanEnterRoom(ABBCharacterBase* Character);
	void SetDiscovered();

	template<typename Type>
	Type* GetRoomFunctionAs() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
		UBBRoomFunctionComponent* GetRoomFunction() const;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		class UBBRoomInfoList* RoomInfoList;
	UFUNCTION(BlueprintCallable, BlueprintPure)
		class UBBRoomInfo* GetRoomInfo() const;
private:
	UPROPERTY(Category = Walls, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMesh* WallMesh;
	UPROPERTY(Category = Walls, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMesh* DoorMesh;
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (AllowPrivateAccess = "true"))
		bool ItemWidgetVisible();
public:
	int32 SafeCombinationNumber = 0;
	bool bSafeCombinationNumberCracked = false;
	bool bIsSelected = false;
	bool bIsHovered = false;
	bool bHasAlarm = false;
	static TArray<int32> AmountAlarms;
	UPROPERTY(BlueprintAssignable)
		FOnBoolChange OnAlarmChanged;
	UFUNCTION(BlueprintCallable)
		bool IsAlarmRoom();

	TArray<ELootType> LootInRoom;
	TArray<EToolType> ToolsInRoom;
	UPROPERTY(BlueprintReadOnly)
		int32 AmountStealthTokens = 0;

	void AddLootToRoom(ELootType Loot);
	void AddToolToRoom(EToolType Tool);
	void RemoveLootFromRoom(ELootType Loot);
	void RemoveToolToRoom(EToolType Tool);
	void SetRavenInRoom(bool HasRaven, bool IsAltRaven);

	bool bContainsRavenAlt = false;
	bool bContainsRaven = false;

	UFUNCTION(BlueprintCallable)
		void SetAlarm(bool HasAlarm);

	UPROPERTY(BlueprintAssignable)
		FOnAction OnRoomItemChanged;

	void SetSelected(bool IsSelected);
	void SetHovered(bool IsHovered);

	UPROPERTY(BlueprintReadOnly)
		class ABBLevel* CurrentLevel;

	bool bHasStairsDown = false;
	bool bHasStairsUp = false;
	void SetHasStairsDown(bool HasStairs);
	void SetHasStairsUp(bool HasStairs);
	UFUNCTION(BlueprintCallable)
		void MoveDown(class ABBPlayerCharacterBase* Character);
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		ERoomType GetRoomType();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool HasAlarm();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsDiscovered();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool HasStairsUp();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool HasStairsDown();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsSafeCombinationNumberCracked();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool ContainsRaven(bool IsAltRaven);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetSafeCombinationNumber();
	// Returns copy
	UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<ELootType> GetLootInRoom();
	// Returns copy
	UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<EToolType> GetToolsInRoom();
};

template<typename Type>
Type* ABBRoomBase::GetRoomFunctionAs() const
{
	return Cast<Type>(RoomFunction);
}
