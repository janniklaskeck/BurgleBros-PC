// Fill out your copyright notice in the Description page of Project Settings.

#include "BBEventComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBEventLibrary.h"
#include "BBLevel.h"
#include "BBRoomBase.h"

void UBBEventComponent::InitEvent(EEventType EventType)
{
	Type = EventType;
	UBBEventLibrary::ActiveEvents.Add(Type);
	switch (Type)
	{
	case EEventType::TimeLock:
		GetOwnerCharacter()->OnTurnStart.AddDynamic(this, &UBBEventComponent::DisableTimeLock);
		break;
	case EEventType::Gymnastics:
		{
			GetOwnerCharacter()->OnTurnStart.AddDynamic(this, &UBBEventComponent::DisableGymnastics);
			TArray<ABBRoomBase *> WalkwayRooms = GetOwnerCharacter()->CurrentLevel->GetRoomsOfType(ERoomType::Walkway);
			for (ABBRoomBase* Room : WalkwayRooms)
			{
				Room->bHasStairsUp = true;
			}
		}
		break;
	case EEventType::VideoLoop:
		{
			GetOwnerCharacter()->OnTurnStart.AddDynamic(this, &UBBEventComponent::DisableVideoLoop);
			TArray<ABBRoomBase*> CameraRooms = GetOwnerCharacter()->CurrentLevel->GetRoomsOfType(ERoomType::Camera);
			for (ABBRoomBase* CameraRoom : CameraRooms)
			{
				CameraRoom->GetRoomFunction()->bIsRoomEnabled = false;
			}
		}
		break;
	default:
		break;
	}
}

void UBBEventComponent::DisableTimeLock()
{
	UBBEventLibrary::ActiveEvents.Remove(EEventType::TimeLock);
	GetOwnerCharacter()->OnTurnStart.RemoveDynamic(this, &UBBEventComponent::DisableTimeLock);
	DestroyComponent();
}

void UBBEventComponent::DisableGymnastics()
{
	TArray<ABBRoomBase *> WalkwayRooms = GetOwnerCharacter()->CurrentLevel->GetRoomsOfType(ERoomType::Walkway);
	for (ABBRoomBase* Room : WalkwayRooms)
	{
		Room->bHasStairsUp = false;
	}
	UBBEventLibrary::ActiveEvents.Remove(EEventType::Gymnastics);
	GetOwnerCharacter()->OnTurnStart.RemoveDynamic(this, &UBBEventComponent::DisableGymnastics);
	DestroyComponent();
}

void UBBEventComponent::DisableVideoLoop()
{
	UBBEventLibrary::ActiveEvents.Remove(EEventType::VideoLoop);
	TArray<ABBRoomBase*> CameraRooms = GetOwnerCharacter()->CurrentLevel->GetRoomsOfType(ERoomType::Camera);
	for (ABBRoomBase* CameraRoom : CameraRooms)
	{
		CameraRoom->GetRoomFunction()->bIsRoomEnabled = true;
	}
	GetOwnerCharacter()->OnTurnStart.RemoveDynamic(this, &UBBEventComponent::DisableVideoLoop);
	DestroyComponent();
}

class ABBPlayerCharacterBase* UBBEventComponent::GetOwnerCharacter()
{
	return Cast<ABBPlayerCharacterBase>(GetOwner());
}
