// Fill out your copyright notice in the Description page of Project Settings.

#include "BBServiceDuctComponent.h"
#include "BBRoomBase.h"
#include "BBPlayerCharacterBase.h"
#include "BBLevel.h"

bool UBBServiceDuctComponent::CanUse(ABBCharacterBase* Character)
{
	UBBRoomFunctionComponent* OtherServiceDuct = GetOtherServiceDuct();
	ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
	return OtherServiceDuct 
		&& OtherServiceDuct->GetOwnerRoom()->bIsDiscovered 
		&& Super::CanUse(Character) 
		&& Character->Actions.GetCurrentActions() > 0 
		&& !PlayerCharacter->Loot.Contains(ELootType::Painting);
}

void UBBServiceDuctComponent::OnUsed(ABBCharacterBase* Character)
{
	GetOwnerRoom()->LeaveRoom(Character);
	UBBRoomFunctionComponent* OtherServiceDuct = GetOtherServiceDuct();
	OtherServiceDuct->GetOwnerRoom()->EnterRoom(Character, true);
	OtherServiceDuct->GetOwnerRoom()->TeleportToRoom(Character);
}

UBBRoomFunctionComponent* UBBServiceDuctComponent::GetOtherServiceDuct()
{
	for (ABBRoomBase* ServiceDuctRoom : GetOwnerRoom()->CurrentLevel->GetRoomsOfType(ERoomType::ServiceDuct))
	{
		if (ServiceDuctRoom->GetRoomFunction() != this)
		{
			return ServiceDuctRoom->GetRoomFunction();
		}
	}
	return nullptr;
}
