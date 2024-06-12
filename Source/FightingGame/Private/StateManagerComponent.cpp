// Fill out your copyright notice in the Description page of Project Settings.


#include "StateManagerComponent.h"

// Sets default values for this component's properties
UStateManagerComponent::UStateManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bAutoRegister = true;
}


// Called when the game starts
void UStateManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	

	// ...
	
}


// Called every frame
void UStateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bCanTickState) 
	{
		currentState->TickState();
	}

	if (bDebug) 		
	{
		if(currentState->IsValidLowLevel())
		{
		}
			
	}
}



void UStateManagerComponent::SwitchStateByKey(FString StateKey)
{
	/*Bind State*/
	UStateBase* NewState = StateMap.FindRef(StateKey);

	/*Check if state is valid*/
	if (NewState->IsValidLowLevel())
	{
		/*If there is no current state then we are in initialization stage*/
		if (!currentState)
		{
			currentState = NewState;
		}
		else
		{
			if (currentState->GetClass() == NewState->GetClass() && currentState->bCanRepeate == false)
			{
				if (bDebug)
				{			
					UE_LOG(LogTemp, Warning, TEXT("State is not repeatable!"));
				}

			}
			else
			{
				/*Will eventually use the check transition function to see if the state can be entered.*/
				if(NewState->CheckTransition())
				{
					bCanTickState = false;
					currentState->OnExitState();
					StateHistory.Push(currentState);
					currentState = NewState;
				}
		
			}
		}
		if (currentState)
		{
			if (bDebug)
			{
				UE_LOG(LogTemp, Warning, TEXT("State switch success!"));
			}
			/*Initialize Current State*/
			currentState->OnEnterState(GetOwner());
			bCanTickState = true;
		}


	}
	else
	{		
		if (currentState->IsValidLowLevelFast())
		{
			UE_LOG(LogTemp, Warning, TEXT("State is not valid!"));
		}
	}
}



void UStateManagerComponent::InitStateManager()
{
	/*Original to the Tutorial. Sets the State to the Initial State*/
	SwitchStateByKey(InitialState);

}

void UStateManagerComponent::InitializeStates()
{
	/*Create Instances of each state and add them to the state map*/
	for (auto It = AvailableStates.CreateConstIterator(); It; ++It)
	{
		if(It->Value->IsValidLowLevel())
		{
			UStateBase* State = NewObject<UStateBase>(this, It->Value);
			FString Key = It->Key;			
			StateMap.Add(It->Key, State);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Provided State is invalid!"));
		}
		
	}
}



