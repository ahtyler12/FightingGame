// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandListEntry.h"
#include "../FightingGameCharacter.h"

float UCommandListEntry::GetMeterCost()
{
	return meterCost;
}

void UCommandListEntry::ResetVariables()
{
	currentComboUses = 0;
	damageScalingModifier = 1.0; //May need to be set from the current combo as opposed to the move itself
	
}

