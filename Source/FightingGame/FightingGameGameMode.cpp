// Copyright Epic Games, Inc. All Rights Reserved.

#include "FightingGameGameMode.h"
#include "FightingGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFightingGameGameMode::AFightingGameGameMode()
{
	//// set default pawn class to our Blueprinted character
	////static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Game/ Content / Characters / Mutant / BP_MutantCharacter.uasset"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}

AFightingGameCharacter* AFightingGameGameMode::getOtherPlayer(AFightingGameCharacter* requester)
{
	if (requester == player1)
		return player2;
	else
		return player1;
}
