// Fill out your copyright notice in the Description page of Project Settings.

#include "BBRoomInfoList.h"

UBBRoomInfo* UBBRoomInfoList::GetRoomByType(ERoomType Room)
{
	for (UBBRoomInfo* RoomInfo : RoomList)
	{
		if (RoomInfo->Type == Room)
		{
			return RoomInfo;
		}
	}
	return nullptr;
}
