// Fill out your copyright notice in the Description page of Project Settings.


#include "FightingGameController.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"

AFightingGameController::AFightingGameController()
{

}
void AFightingGameController::BeginPlay()
{
	Super::BeginPlay();
	possessedPawn = Cast<AFightingGameCharacter>(GetPawn());
	isInputDeviceGamepad = false;

}

void AFightingGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!possessedPawn)
	{
		possessedPawn = Cast<AFightingGameCharacter>(GetPawn());
	}		
}

void AFightingGameController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (auto gamemode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
	{

		if (GetInputIndex() <= 0)
		{
			InputComponent->BindAxis("MoveRight", this, &AFightingGameController::PawnMove);
		}
	}

		InputComponent->BindAxis("MoveRightPad", this, &AFightingGameController::PawnMoveController);
		InputComponent->BindAction("Jump", IE_Pressed, this, &AFightingGameController::PawnJump);
		InputComponent->BindAction("LightAttack", IE_Pressed, this, &AFightingGameController::PawnLightAttack);
		InputComponent->BindAction("MediumAttack", IE_Pressed, this, &AFightingGameController::PawnMediumAttack);
		InputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AFightingGameController::PawnHeavyAttack);
		InputComponent->BindAction("SpecialAttack", IE_Pressed, this, &AFightingGameController::PawnSpecialAttack);
		InputComponent->BindAction("Throw", IE_Pressed, this, &AFightingGameController::PressMacro1);
		InputComponent->BindAction("Block", IE_Pressed, this, &AFightingGameController::PawnBlock);
		InputComponent->BindAction("LightAttack", IE_Released, this, &AFightingGameController::ReleasePawnLightAttack);
		InputComponent->BindAction("MediumAttack", IE_Released, this, &AFightingGameController::ReleasePawnMediumAttack);
		InputComponent->BindAction("HeavyAttack", IE_Released, this, &AFightingGameController::ReleasePawnHeavyAttack);
		InputComponent->BindAction("SpecialAttack", IE_Released, this, &AFightingGameController::ReleasePawnSpecialAttack);
		InputComponent->BindAction("Throw", IE_Released, this, &AFightingGameController::ReleaseMacro1);
		InputComponent->BindAction("Block", IE_Pressed, this, &AFightingGameController::PawnStopBlock);		
		InputComponent->BindAction("Crouch", IE_Pressed, this, &AFightingGameController::PawnCrouch);
		InputComponent->BindAction("Crouch", IE_Released, this, &AFightingGameController::PawnStopCrouch);
		InputComponent->BindAction("Pause", IE_Pressed, this, &AFightingGameController::PerformStartInputLogic);

		FInputActionBinding& anyKeyBinding = InputComponent->BindAction("AnyKey", IE_Pressed, this, &AFightingGameController::DetermineInputDeviceDetails);
		anyKeyBinding.bConsumeInput = false;
		for (int i = 0; i < InputComponent->GetNumActionBindings(); ++i)
		{
			InputComponent->GetActionBinding(i).bConsumeInput = false;
		}

		//Get all Action Mapping Names
		TArray<FName> actionMappingNames;
		GetDefault<UInputSettings>()->GetActionNames(actionMappingNames);

		//Get All Axis Mapping Names
		TArray<FName> axisMappingNames;
		GetDefault<UInputSettings>()->GetAxisNames(axisMappingNames);

		//Find Player 2 action mappings
		for (int i = 0; i < actionMappingNames.Num(); ++i)
		{
			if (actionMappingNames[i].ToString().Contains("P2"))
			{
				keyboardSpecificActionNames.Add(actionMappingNames[i]);
			}
		}

		//Find Player 2 axis mappings
		for (int i = 0; i < axisMappingNames.Num(); ++i)
		{
			if (axisMappingNames[i].ToString().Contains("P2"))
			{
				keyboardSpecificActionNames.Add(axisMappingNames[i]);
			}
		}

	
}

void AFightingGameController::PawnMove(float _value)
{
	if (possessedPawn) {		
		possessedPawn->MoveRight(_value);
	}

	
}

void AFightingGameController::PawnMoveController(float _value)
{
	if (possessedPawn) {
		possessedPawn->MoveRightPad(_value);
	}

}

void AFightingGameController::PawnJump()
{
	if (possessedPawn)
		possessedPawn->Jump();
}

void AFightingGameController::PawnLightAttack()
{
	if (possessedPawn)
		possessedPawn->StartLightAttack();
		
}

void AFightingGameController::ReleasePawnLightAttack()
{
	if (possessedPawn)
		possessedPawn->ReleaseLightAttack();

}

void AFightingGameController::PawnMediumAttack()
{
	if (possessedPawn)
		possessedPawn->StartMediumAttack();
		
}

void AFightingGameController::ReleasePawnMediumAttack()
{
	if (possessedPawn)
		possessedPawn->ReleaseMediumAttack();

}

void AFightingGameController::PawnHeavyAttack()
{
	if (possessedPawn)
		possessedPawn->StartHeavyAttack();
}

void AFightingGameController::ReleasePawnHeavyAttack()
{
	if (possessedPawn)
		possessedPawn->ReleaseHeavyAttack();
}

void AFightingGameController::PawnSpecialAttack()
{
	if (possessedPawn)
		possessedPawn->StartSpecialAttack();
}

void AFightingGameController::ReleasePawnSpecialAttack()
{
	if (possessedPawn)
		possessedPawn->ReleaseSpecialAttack();
}

void AFightingGameController::PawnBlock()
{
	if (possessedPawn)
		possessedPawn->StartBlocking();
}

void AFightingGameController::PawnStopBlock()
{
	if (possessedPawn)
		possessedPawn->StopBlocking();
}

void AFightingGameController::PawnCrouch()
{
	if (possessedPawn)
		possessedPawn->StartCrouching();
}

void AFightingGameController::PawnThrow()
{
	if (possessedPawn)
		possessedPawn->StartThrow();
}

void AFightingGameController::PressMacro1()
{
	if (possessedPawn)
	{
		possessedPawn->PressMacro1();
	}
}

void AFightingGameController::ReleaseMacro1()
{
	if (possessedPawn)
	{
		possessedPawn->ReleaseMacro1();
	}
}

void AFightingGameController::PerformStartInputLogic()
{
	if (auto gameMode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (gameMode->matchState == EMatchState::E_StageIntro)
		{
			gameMode->skipStageIntro();
		}
		else if (gameMode->matchState == EMatchState::E_CharacterIntroA || gameMode->matchState == EMatchState::E_CharacterIntroB)
		{
			gameMode->skipPlayerIntro();
			if (gameMode->shouldSkipAllIntros)
			{
				gameMode->matchState = EMatchState::E_MatchBegin;
			}
			else
			{
				if (gameMode->matchState == EMatchState::E_CharacterIntroA)
				{ 
					gameMode->matchState = EMatchState::E_CharacterIntroB;
				}
				else if (gameMode->matchState == EMatchState::E_CharacterIntroB)
				{
					gameMode->matchState = EMatchState::E_MatchBegin;
				}
			}
		}
		else if(gameMode->matchState == EMatchState::E_MatchActive)
		{
			gameMode->PauseGame(this);
		}
	}
}

void AFightingGameController::DetermineInputDeviceDetails(FKey _keyPressed)
{
	bool foundKeyboardKey = false;

	//Check if the key pressed is a included in a keyboard only input action
	for (int i = 0; i < keyboardSpecificActionNames.Num(); ++i)
	{
		TArray<FInputActionKeyMapping> keyforAction = PlayerInput->GetKeysForAction(keyboardSpecificActionNames[i]);
		for (int j = 0; j < keyforAction.Num(); ++j)
		{
			if (keyforAction[j].Key == _keyPressed)
			{
				foundKeyboardKey = true;
				break;
			}
		}

	}
	
	//Check if the key pressed is a included in a keyboard only input axis
	for (int i = 0; i < keyboardSpecificAxisNames.Num(); ++i)
	{
		TArray<FInputAxisKeyMapping> keyforAxis = PlayerInput->GetKeysForAxis(keyboardSpecificActionNames[i]);
		for (int j = 0; j < keyforAxis.Num(); ++j)
		{
			if (keyforAxis[j].Key == _keyPressed)
			{
				foundKeyboardKey = true;
				break;
			}
		}

	}

	for (int i = 0; i < keyboardSpecificAxisNames.Num(); ++i)
	{
		TArray<FInputAxisKeyMapping> keysForAxis = PlayerInput->GetKeysForAxis(keyboardSpecificAxisNames[i]);

		for (int j = 0; j < keysForAxis.Num(); ++j)
		{
			if (keysForAxis[j].Key == _keyPressed)
			{
				foundKeyboardKey = true;
				break;
			}
		}
	}

	if (auto gameMode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (auto player2REF = gameMode->player2)
		{
			
			if (Cast<AFightingGameController>(player2REF->GetController())->isInputDeviceGamepad && foundKeyboardKey)
			{
				if (_keyPressed.IsGamepadKey())
				{
					Cast<AFightingGameController>(player2REF->GetController())->isInputDeviceGamepad = true;
				}
				else
				{
					Cast<AFightingGameController>(player2REF->GetController())->isInputDeviceGamepad = false;
				}
			}
			else
			{
				if (!foundKeyboardKey)
				{
					if (_keyPressed.IsGamepadKey())
					{
						isInputDeviceGamepad = true;
					}
					else
					{
						isInputDeviceGamepad = false;
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to get reference to Player2!!!!"));
		}
	}

}

void AFightingGameController::ClearPossessedPawn()
{
	possessedPawn = nullptr;
}

void AFightingGameController::PawnStopCrouch()
{
	if (possessedPawn)
	{
		possessedPawn->StopCrouching();
	}
}

