// Fill out your copyright notice in the Description page of Project Settings.


#include "StateBase.h"

#pragma optimize("", off)

void UStateBase::OnEnterState_Implementation(AActor* OwningActor)
{
}

void UStateBase::OnExitState_Implementation()
{
}

void UStateBase::TickState_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Ticking current state!"));
}

bool UStateBase::CheckTransition_Implementation()
{
	return false;
}
