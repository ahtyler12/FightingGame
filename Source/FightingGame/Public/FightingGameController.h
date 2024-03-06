// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../FightingGameCharacter.h"
#include "../FightingGameGameMode.h"
#include "FightingGameController.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGGAME_API AFightingGameController : public APlayerController
{
	GENERATED_BODY()


public:
	AFightingGameController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
		void PawnMove(float _value);
	UFUNCTION(BlueprintCallable)
		void PawnMoveController(float _value);
	UFUNCTION(BlueprintCallable)
		void PawnJump();
	UFUNCTION(BlueprintCallable)
		void PawnLightAttack();
	UFUNCTION(BlueprintCallable)
		void ReleasePawnLightAttack();
	UFUNCTION(BlueprintCallable)
		void PawnMediumAttack();
	UFUNCTION(BlueprintCallable)
		void ReleasePawnMediumAttack();
	UFUNCTION(BlueprintCallable)
		void PawnHeavyAttack();
	UFUNCTION(BlueprintCallable)
		void ReleasePawnHeavyAttack();
	UFUNCTION(BlueprintCallable)
		void PawnSpecialAttack();
	UFUNCTION(BlueprintCallable)
		void ReleasePawnSpecialAttack();
	UFUNCTION(BlueprintCallable)
		void PawnBlock();
	UFUNCTION(BlueprintCallable)
		void PawnStopBlock();
	UFUNCTION(BlueprintCallable)
		void PawnCrouch();
	UFUNCTION(BlueprintCallable)
		void PawnStopCrouch();
	UFUNCTION(BlueprintCallable)
		void PawnThrow();
	UFUNCTION(BlueprintCallable)
	void PressMacro1();
	UFUNCTION(BlueprintCallable)
	void ReleaseMacro1();	
	UFUNCTION(BlueprintCallable)
		void PerformStartInputLogic();
	UFUNCTION(BlueprintCallable)
		void ClearPossessedPawn();


	void DetermineInputDeviceDetails(FKey _keyPressed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contorller")
		bool isInputDeviceGamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contorller")
		TArray<FName> keyboardSpecificAxisNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contorller")
		TArray<FName> keyboardSpecificActionNames;
	
	//Pawn Possessed by controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contorller")
		AFightingGameCharacter* possessedPawn;
	
};
