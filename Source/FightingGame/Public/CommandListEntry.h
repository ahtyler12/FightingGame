// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enums.h"
#include "Structs.h"
#include "CommandListEntry.generated.h"

/**
 * 
 */






UCLASS(Blueprintable)
class FIGHTINGGAME_API UCommandListEntry : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	//List of Valid extensions to this command. Such as Rekka type moves and combo trees.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
		TArray<UCommandListEntry*> Branches;

	//Required Meter for move to be performed. -1 means there is no meter cost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
		float meterCost = -1;

	//In the event that a move does not return to Idle the character will instead return to this move instead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
		UCommandListEntry* ReturnMove;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	bool LoopAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FCommand commandData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		float damageScalingModifier;

	//The maximum amount of scaling an attack can have before it overrides the value in the damage function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		float maxScaling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		TArray<FName> hitboxRowNames;
	//May not be needed. The maximum times this may be used in a combo before it forces a drop
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		int maxComboUses;
	//Current number of times used in combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		int currentComboUses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		int commandPriority;
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetMeterCost();
	//Returns True if Character is in proper state to Execute Command
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		bool CheckState(ECharacterState _currentState, EComboState _comboState);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void StartCommand();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void EndCommand();
	UFUNCTION(BlueprintCallable)
		void ResetVariables();
};
