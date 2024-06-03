// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Engine/DataTable.h"
#include "Structs.generated.h"

USTRUCT(BlueprintType)
struct FCommandInput
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EInputType inputType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EInputState inputState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int64 requiredChargeFrames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool isCurrentlyHeld;
};

USTRUCT(BlueprintType)
struct FCommand : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<FCommandInput> inputTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	ECharacterState requiredState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	ECharacterState resulatingState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EComboType comboType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float meterCost = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int64 maxInputWindow = 12;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool isCharging;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int currentFramesHeld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int maxFramesHeld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int commandPriority;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool hasUsedCommand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool hasAutoCombo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int stateID;
};


/**
 * 
 */
class FIGHTINGGAME_API Structs
{
public:

};
