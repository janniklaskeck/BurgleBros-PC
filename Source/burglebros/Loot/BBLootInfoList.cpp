// Fill out your copyright notice in the Description page of Project Settings.

#include "BBLootInfoList.h"

UBBLootInfo* UBBLootInfoList::GetLootByType(ELootType Loot)
{
	for (UBBLootInfo* LootInfo : LootList)
	{
		if (LootInfo->Type == Loot)
		{
			return LootInfo;
		}
	}
	return nullptr;
}
