// Fill out your copyright notice in the Description page of Project Settings.

#include "BBToolList.h"

UBBToolInfo* UBBToolList::GetToolByType(EToolType ToolType)
{
	for (UBBToolInfo* ToolInfo : ToolList)
	{
		if (ToolInfo->Type == ToolType)
		{
			return ToolInfo;
		}
	}
	return nullptr;
}
