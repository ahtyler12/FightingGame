// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Character.h"
#include "Structs.h"
#include "Enums.h"
#include "State.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FIGHTINGGAME_API UState : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	
	UState();
	

	/*
		VARIABLES
	*/

	//State ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	int stateID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	bool canTickState = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	bool canRepeat = false;

	//Reference to owning Character 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	ACharacter* owningChar;

	//Animation to Play during state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	UAnimMontage* AnimMontage;

	//Character States that this move can be performed from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<ECharacterState> acceptableCharacterState;

	//Combo States That this move can be performed from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<EComboState> acceptableComboState;

	//States that branch from this state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	TArray<UState*> Branches;

	//State to Return to if no branch is taken 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
	UState* returnState;


	/*
	     Functions
	*/
	// Plays Animation if valid. Sets any variables related to state
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EnterState();

	//Cleans up state data
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ExitState();

	//Called from State Manager and checks if character is in the proper state to enter this state.Returns true or false
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool CheckTransition();

	//Runs any checks while in state if state is set to tick
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TickState();

};
