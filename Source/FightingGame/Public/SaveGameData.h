// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameData.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGGAME_API USaveGameData : public USaveGame
{
	GENERATED_BODY()
public:
	USaveGameData();


	UPROPERTY(VisibleAnywhere)
	int currency;
};
