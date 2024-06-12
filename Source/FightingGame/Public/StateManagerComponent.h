// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateBase.h"
#include "StateManagerComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGGAME_API UStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		


	//Current State the character is in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	UStateBase* currentState;

	//List of states that are available to this character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	TMap<FString, TSubclassOf<UStateBase>> AvailableStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	TMap<FString, UStateBase*> StateMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	TArray<UStateBase*> StateHistory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	FString InitialState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 stateHistoryLength = 10;



	UFUNCTION(BlueprintCallable)
	void SwitchStateByKey(FString StateKey);


	UFUNCTION(BlueprintCallable)
	void InitStateManager();

private:
	bool bCanTickState = true;
	void InitializeStates();
		
};
