// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBRoomBase.h"
#include "BBDelegates.h"
#include "BBLevel.generated.h"

USTRUCT(BlueprintType)
struct FRoomClass
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<ABBRoomBase> RoomClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 AmountAvailable = 3;
};

USTRUCT(BlueprintType)
struct FFloor
{
	GENERATED_BODY()
public:
	FFloor()
	{
	}
	FFloor(int32 Index)
	{
		FloorIndex = Index;
	}
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		int32 FloorIndex = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<ABBRoomBase*> Rooms;

	UPROPERTY(BlueprintReadOnly)
		bool bIsRevealed = false;

	UPROPERTY(BlueprintAssignable)
		FOnIntChange OnFloorRevealed;

	void RevealFloor()
	{
		if (!bIsRevealed)
		{
			bIsRevealed = true;
			OnFloorRevealed.Broadcast(FloorIndex);
			UE_LOG(LogTemp, Warning, TEXT("Floor %d revealed!"), FloorIndex);
		}
	}

	void SetFloorHidden(bool IsHidden)
	{
		for (ABBRoomBase* Room : Rooms)
		{
			Room->SetHidden(IsHidden);
		}
	}
};

UCLASS(Blueprintable)
class BURGLEBROS_API ABBLevel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABBLevel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 FloorWidth = 4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 AmountWalls = 8;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 AmountFloors = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float TileWidth = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float FloorHeight = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDebugDraw = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TMap<ERoomType, FRoomClass> AllRoomClasses;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TSubclassOf<ABBRoomBase>> GuaranteedRoomClasses;
private:
	TArray<ERoomType> AvailableRoomClasses;
	TArray<TSubclassOf<ABBRoomBase>> GetFloorDeck();
public:
	void BuildLevel();

	FFloor& GetFloor(int32 Index);
	UFUNCTION(BlueprintCallable)
		ABBRoomBase* GetRoomAt(FRoomPosition RoomPosition);
	ABBRoomBase* GetRoomAt(int32 PosX, int32 PosY, int32 Floor);
	TArray<ABBRoomBase*> GetRoomsOfFloor(int32 Floor);
	TArray<ABBRoomBase*> GetRoomsOfFloor(class ABBCharacterBase* Character);
	UFUNCTION(BlueprintCallable, Category = Rooms)
		TArray<ABBRoomBase*> GetRoomsOfType(ERoomType RoomType);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		ABBRoomBase* GetNeighbourRoomInDirection(ABBRoomBase* OriginalRoom, EDirection Direction);

	UFUNCTION(BlueprintCallable)
		TArray<class ABBPlayerCharacterBase*> GetPlayersOnFloor(int32 FloorIndex);

	FRoomPosition GetRandomRoomPositionOnFloor(int32 Floor);
private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		TArray<FFloor> Floors;

	void BuildFloors();
	void PlaceTiles(int32 Floor);
	void PlaceWalls(int32 Floor);
	bool CanPlaceWallAt(ABBRoomBase* Room, EDirection Side);
	void PlaceWallInRoom(ABBRoomBase* Room, EDirection Side);

	UFUNCTION()
		void SpawnGuardInRevealedFloor(int32 Floor);
public:
	UFUNCTION(BlueprintCallable)
		void DiscoverAllRooms();
};
