// Fill out your copyright notice in the Description page of Project Settings.

#include "BBLaboratoryComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBToolLibrary.h"
#include "BBUtilities.h"
#include "BBMainGameMode.h"

bool UBBLaboratoryComponent::OnEntered(class ABBCharacterBase* Character)
{
	Super::OnEntered(Character);
	ABBPlayerCharacterBase* PlayerCharacter = Cast<ABBPlayerCharacterBase>(Character);
	if (PlayerCharacter && !bAlreadyVisited)
	{
		UBBUtilities::GetMainGameMode(this)->EnterDrawCard();
		bAlreadyVisited = true;
	}
	return true;
}

