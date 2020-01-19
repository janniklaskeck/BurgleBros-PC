// Fill out your copyright notice in the Description page of Project Settings.

#include "BBPlayerClassInfo.h"

FText UBBPlayerClassInfo::GetClassAbilityName(EPlayerClass SelectedPlayerClass)
{
	if (SelectedPlayerClass == EPlayerClass::Acrobat
		|| SelectedPlayerClass == EPlayerClass::Hacker
		|| SelectedPlayerClass == EPlayerClass::Hawk
		|| SelectedPlayerClass == EPlayerClass::Juicer
		|| SelectedPlayerClass == EPlayerClass::Peterman
		|| SelectedPlayerClass == EPlayerClass::Raven
		|| SelectedPlayerClass == EPlayerClass::Rigger
		|| SelectedPlayerClass == EPlayerClass::Rook
		|| SelectedPlayerClass == EPlayerClass::Spotter)
	{
		return AbilityName;
	}
	return AbilityNameAlt;
}

FText UBBPlayerClassInfo::GetClassAbilityDescription(EPlayerClass SelectedPlayerClass)
{
	if (SelectedPlayerClass == EPlayerClass::Acrobat
		|| SelectedPlayerClass == EPlayerClass::Hacker
		|| SelectedPlayerClass == EPlayerClass::Hawk
		|| SelectedPlayerClass == EPlayerClass::Juicer
		|| SelectedPlayerClass == EPlayerClass::Peterman
		|| SelectedPlayerClass == EPlayerClass::Raven
		|| SelectedPlayerClass == EPlayerClass::Rigger
		|| SelectedPlayerClass == EPlayerClass::Rook
		|| SelectedPlayerClass == EPlayerClass::Spotter)
	{
		return AbilityDescription;
	}
	return AbilityDescriptionAlt;
}
