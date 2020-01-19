// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBLootInfo.h"
#include "Components/ActorComponent.h"
#include "BBLootComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BURGLEBROS_API UBBLootComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	void InitLoot(ELootType Type);
	void RemoveLoot();

	UPROPERTY(BlueprintReadOnly)
		ELootType Type;

private:
	class ABBPlayerCharacterBase* GetOwnerCharacter();
	UFUNCTION()
		void CheckWithTiara(class ABBRoomBase* RoomEntered);
	UFUNCTION()
		void CheckChihuahua();
	UFUNCTION()
		void OnChihuahuaChecked();
	UFUNCTION()
		void CheckPersianKitty();
	UFUNCTION()
		void OnPersianKittyChecked();
	class ABBRoomBase* GetNearestAlarmTile();
};
