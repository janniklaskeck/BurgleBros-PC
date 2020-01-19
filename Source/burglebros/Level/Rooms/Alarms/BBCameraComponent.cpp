// Fill out your copyright notice in the Description page of Project Settings.

#include "BBCameraComponent.h"
#include "BBRoomBase.h"
#include "BBPlayerCharacterBase.h"
#include "BBLevel.h"

bool UBBCameraComponent::OnEntered(ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	if (!GetOwnerRoom()->bIsDiscovered)
		return true;
	TArray<ABBRoomBase*> AllCameras = GetOwnerRoom()->CurrentLevel->GetRoomsOfType(ERoomType::Camera);
	for (ABBRoomBase* CameraRoom : AllCameras)
	{
		if (CameraRoom != GetOwnerRoom() && CameraRoom->bIsDiscovered)
		{
			if (IsGuard(Character))
			{
				for (ABBCharacterBase* CharacterInRoom : CameraRoom->CharactersInRoom)
				{
					if (!IsGuard(CharacterInRoom) && !Cast<ABBPlayerCharacterBase>(CharacterInRoom)->bIsInvisible)
					{
						CameraRoom->SetAlarm(true);
						break;
					}
				}
			}
			else
			{
				for (ABBCharacterBase* CharacterInRoom : CameraRoom->CharactersInRoom)
				{
					if (IsGuard(CharacterInRoom) && !Cast<ABBPlayerCharacterBase>(Character)->bIsInvisible)
					{
						GetOwnerRoom()->SetAlarm(true);
						break;
					}
				}
			}
		}
	}
	return true;
}

bool UBBCameraComponent::CanDisableRoom()
{
	return true;
}
