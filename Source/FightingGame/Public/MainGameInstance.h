// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	VE_Default UMETA(DisplayName = "Mannequin"),
	VE_Mutant  UMETA(DisplayName = "Mutant"),
	VE_Vampire UMETA(DisplayName = "Vampire"),
	VE_Paladin UMETA(DisplayName = "Paladin"),
	VE_Erika   UMETA(DisplayName = "Erika")
};

UCLASS()
class FIGHTINGGAME_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerReferences")
		ECharacterClass characterClass;
};
