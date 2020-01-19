// Fill out your copyright notice in the Description page of Project Settings.

#include "BBPlayerClassInfoList.h"

UBBPlayerClassInfo* UBBPlayerClassInfoList::GetPlayerClassInfo(EPlayerClass PlayerClass)
{
	for (UBBPlayerClassInfo* ClassInfo : ClassInfoList)
	{
		if (ClassInfo->PlayerClass == PlayerClass || ClassInfo->PlayerClassAlt == PlayerClass)
		{
			return ClassInfo;
		}
	}
	return nullptr;
}
