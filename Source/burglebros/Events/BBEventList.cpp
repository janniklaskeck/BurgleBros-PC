// Fill out your copyright notice in the Description page of Project Settings.

#include "BBEventList.h"

UBBEventInfo* UBBEventList::GetEventByType(EEventType Event)
{
	for (UBBEventInfo* EventInfo : EventList)
	{
		if (EventInfo->Type == Event)
		{
			return EventInfo;
		}
	}
	return nullptr;
}
