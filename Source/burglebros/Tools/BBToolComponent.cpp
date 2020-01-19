// Fill out your copyright notice in the Description page of Project Settings.

#include "BBToolComponent.h"
#include "BBPlayerCharacterBase.h"
#include "BBToolLibrary.h"

void UBBToolComponent::InitTool(EToolType ToolType)
{
	Type = ToolType;
	switch (Type)
	{
	case EToolType::EMP:
		UBBToolLibrary::ActiveTools.Add(Type);
		GetOwnerCharacter()->OnTurnStart.AddDynamic(this, &UBBToolComponent::DisableEMP);
		break;
	default:
		break;
	}
}

void UBBToolComponent::DisableEMP()
{
	UBBToolLibrary::ActiveTools.Remove(EToolType::EMP);
	GetOwnerCharacter()->OnTurnStart.RemoveDynamic(this, &UBBToolComponent::DisableEMP);
	DestroyComponent();
}

class ABBPlayerCharacterBase* UBBToolComponent::GetOwnerCharacter()
{
	return Cast<ABBPlayerCharacterBase>(GetOwner());
}
