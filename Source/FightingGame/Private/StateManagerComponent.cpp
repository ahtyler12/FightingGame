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

	InitializeStates();
	

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
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, this->GetOwner()->GetName() + "'s current state is: " + currentState->stateDisplayName.GetPlainNameString());
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
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, this->GetOwner()->GetName() + "'s State Switch Success!: " + currentState->stateDisplayName.GetPlainNameString());
		}
		else
		{
			if (currentState->GetClass() == NewState->GetClass() && currentState->bCanRepeate == false)
			{
				if (bDebug)
				{					
					GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, this->GetOwner()->GetName() + "'s State Switch Failed!: " + currentState->stateDisplayName.GetPlainNameString());
				}

			}
			else
			{
				bCanTickState = false;
				currentState->OnExitState();
				if (StateHistory.Num() < stateHistoryLength)
				{
					StateHistory.Push(currentState);
				}
				else
				{					
					StateHistory.RemoveAt(0);
					StateHistory.Push(currentState);
					
				}
				currentState = NewState;				
			}
		}
		if (currentState)
		{
			if (bDebug)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, this->GetOwner()->GetName() + "'s State Switch Success!: " + currentState->stateDisplayName.GetPlainNameString());
			}
			/*Initialize Current State*/
			currentState->OnEnterState(GetOwner());
			bCanTickState = true;
		}


	}
	else
	{		
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, this->GetOwner()->GetName() + "'s State Switch Failed!: " + currentState->stateDisplayName.GetPlainNameString() + " is not valid!");
	}
}

void UStateManagerComponent::SwitchState(UStateBase* newState)
{
	if (newState->IsValidLowLevel())
	{
		if (!currentState)
		{
			currentState = newState;
		}
		else
		{
			if (currentState->GetClass() == newState->GetClass() && currentState->bCanRepeate == false)
			{
				if (bDebug)
				{
					UE_LOG(LogTemp, Warning, TEXT("State Switch Failed!"));
				}
			}
			else
			{
				bCanTickState = false;
				currentState->OnExitState();
				if (StateHistory.Num() < stateHistoryLength)
				{
					StateHistory.Push(currentState);
				}
				else
				{
					StateHistory.RemoveAt(0);
					StateHistory.Push(currentState);

				}
				if (currentState)
				{
					currentState = newState;
					/*Initialize Current State*/
					currentState->OnEnterState(GetOwner());
					bCanTickState = true;
				}
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, this->GetOwner()->GetName() + "'s State Switch Failed!: " + newState->stateDisplayName.GetPlainNameString() + " is not valid!");
	}
}

void UStateManagerComponent::InitStateManager()
{
	/*Original to the Tutorial. Sets the State to the Initial State*/
	SwitchStateByKey(InitialState);
}

void UStateManagerComponent::InitializeStates()
{
	for (auto It = AvailableStates.CreateConstIterator(); It; ++It)
	{
		if(It->Value->IsValidLowLevel())
		{
			UStateBase* State = NewObject<UStateBase>(this, It->Value);
			StateMap.Add(It->Key, State);
		}
		
	}
}



