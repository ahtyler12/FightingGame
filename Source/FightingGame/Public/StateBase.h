// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FIGHTINGGAME_API UStateBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanTickState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRepeate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName stateDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int stateID;

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnEnterState(AActor* OwningActor);

	UFUNCTION(BlueprintNativeEvent)
	void OnExitState();

	UFUNCTION(BlueprintNativeEvent)
	void TickState();

	UFUNCTION(BlueprintNativeEvent)
	bool CheckTransition();
};
