// Copyright Epic Games, Inc. All Rights Reserved.

#include "FightingGameCharacter.h"
#include "FightingGameGameMode.h"
#include "FightingGameController.h"
#include "HitboxActor.h"
#include "CommandListEntry.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

//////////////////////////////////////////////////////////////////////////
// AFightingGameCharacter
#pragma optimize("", off)

AFightingGameCharacter::AFightingGameCharacter()
{

	StateManager = CreateDefaultSubobject<UStateManagerComponent>(TEXT("State Manager"));




	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);



	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	
	




	maxPlayerHealth = 1.0f;
	playerHealth = maxPlayerHealth;
	hasLandedHit = false;
	hasLandedThrow = false;
	maxSuperMeterAmount = 1.0f;
	superMeterAmount = maxSuperMeterAmount;
	Hurtbox = nullptr;
	otherPlayer = nullptr;
	isFlipped = false;
	scale = FVector(0.0f, 0.0f, 0.0f);
	transform = GetActorTransform();
	characterState = ECharacterState::E_Default;
	comboState = EComboState::E_None;
	attackState = EAttackState::E_NONE;
	invulType = EInvulType::E_None;
	isCrouching = false;
	MaxDistanceApart = 800.0f;
	GetCharacterMovement()->GravityScale = 1.3f;
	gravityScale = GetCharacterMovement()->GravityScale;
	GetCharacterMovement()->bApplyGravityWhileJumping = false;
	gravityscaleModifier = 0.7f;
	stunFrames = 0;
	hitstopFrames = 1;
	isFacingRight = true;
	forwardDashDistance = 800.f;
	backDashDistance = 800.f;
	canMove = true;
	canAttack = true;
	canFlip = true;
	inputDecayTime = 0.20f;
	isPlayerOne = false;
	playingOutro = false;
	playingSuper = false;
	characterClass = ECharacterClass::E_Yusuke;
	lastComboRating = EComboRating::E_Default;
	lastDamageReceived = 0.0f;
	lastStunFramesReceived = 0;

	jumpCount = 0;
	maxJumpCount = 2;
	jumpHeight = 850.0f;
	jumpDistance = 500.0f;

	//Match Flow Variables
	roundLost = false;
	roundsWon = 0;
	canTakeDamage = true;
	wonMatch = false;
	
	
	hitStunDecayModifier = 2.0f;
	damageDecayModifier = 1.5f;
	comboCounter = 0;

	

	currentTick = 0;
	currentFrameInputIndex = 0;
	capturedInputThisFrame = false;

	/* Instant Block */
	numFramesUntilInstantBlockAttempt = 0;
	startInstantBlockFrame = -1;
}

void AFightingGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (auto gameMode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (gameMode->player1 == this)
		{
			isPlayerOne = true;	
		}
		else
		{
			isPlayerOne = false;
		}
	}
}
void AFightingGameCharacter::BeginPlay()
{
	StateManager->InitStateManager();
}
void AFightingGameCharacter::Tick(float DeltaTime)
{
	if (CustomTimeDilation > 0.0f)
	{
		Super::Tick(DeltaTime);
		takenDamageThisFrame = false;

		if (isStuckInEnemy)
		{
			float distanceApart = fabs(GetActorLocation().X - otherPlayer->GetActorLocation().X);
			float distanceToMove = (145.0f - distanceApart) * .5f;

			if (otherPlayer->isFacingRight)
			{
				AddActorWorldOffset(FVector(distanceToMove, 0.0f, 0.0f), true, false);
				otherPlayer->AddActorWorldOffset(FVector(distanceToMove * -1.0f, 0.0f, 0.0f), true, false);
			}
			else
			{
				AddActorWorldOffset(FVector(distanceToMove * -1, 0.0f, 0.0f), true, false);
				otherPlayer->AddActorWorldOffset(FVector(distanceToMove, 0.0f, 0.0f), true, false);
			}

			IgnoreCollision(false);

			isStuckInEnemy = false;
		}



		//Fills Inputbuffer with null inputs if there is no input captured on current frame
		if (!capturedInputThisFrame)
		{
			FInputInfo nullInput;
			nullInput.inputType = EInputType::E_None;
			nullInput.inputState = EInputState::E_None;
			nullInput.inputFrame = GFrameCounter;
			nullInput.chargeFrame = 0;
			inputBuffer[currentTick].inputs[0] = nullInput;
		}
		else
		{
			capturedInputThisFrame = false;
			currentFrameInputIndex = 0;
		}

		for (int i = 0; i < ChargeTimes.Num(); ++i)
		{
			if (ChargeTimes[i].IsHoldingInput)
			{
				++ChargeTimes[i].ChargeFrames;
			}
		}


		//Set Tick for Input checking. Ranges from 0-59
		if (currentTick < 59)
		{
			++currentTick;
		}
		else
		{
			currentTick = 0;
		}

		//Checks if the stun time is active or not
		if (stunFrames > 0)
		{
			--stunFrames;
			if (stunFrames <= 0)
			{
				EndStun();
			}
		}


		//Decrement Counter for Instant Block Spam Protection
		if (numFramesUntilInstantBlockAttempt > 0)
		{
			--numFramesUntilInstantBlockAttempt;
		}

		//Checks Commandlist for any available moves to use
		DetermineCommandToUse();

		if (characterState != ECharacterState::E_NeutralJump
			&& characterState != ECharacterState::E_ForwardJump
			&& characterState != ECharacterState::E_BackwardJump
			&& characterState != ECharacterState::E_WallJump
			&& comboState != EComboState::E_GroundBounce
			&& comboState != EComboState::E_WallBounce
			&& canFlip)
		{
			if (otherPlayer)
			{
				if (auto characterMovement = GetCharacterMovement())
				{
					if (auto enemyMovement = otherPlayer->GetCharacterMovement())
					{
						if (enemyMovement->GetActorLocation().X >= characterMovement->GetActorLocation().X)
						{
							if (!isFacingRight)
							{
								if (auto mesh = GetCapsuleComponent()->GetChildComponent(1))
								{
									transform = mesh->GetRelativeTransform();
									scale = transform.GetScale3D();
									scale.Y = 1;									
									transform.SetScale3D(scale);
									mesh->SetRelativeTransform(transform);

								}
								isFacingRight = true;
							}
						}
						else
						{
							if (isFacingRight)
							{
								if (auto mesh = GetCapsuleComponent()->GetChildComponent(1))
								{
									transform = mesh->GetRelativeTransform();
									scale = transform.GetScale3D();
									scale.Y = -1;
									transform.SetScale3D(scale);
									mesh->SetRelativeTransform(transform);

								}
								isFacingRight = false;
							}
						}
					}
				}
			}
			else
			{
				auto gameMode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode());
				gameMode->getOtherPlayer(this);
			}

		}
	}


}

void AFightingGameCharacter::MoveRight(float Value)
{	
	if (this)
	{
		if (auto baseGameInstance = Cast<UMainGameInstance>(GetGameInstance()))
		{
			if (auto controller = Cast<AFightingGameController>(GetController()))
			{
				
				if (!controller->isInputDeviceGamepad)
				{
					if(characterState != ECharacterState::E_Crouching)
					{
						if (Value > 0.01f)
						{
							if (!hasReleasedLeftAxisInput)
							{
								hasReleasedLeftAxisInput = true;

								if (isFacingRight)
								{
									PerformInputLogic(EInputType::E_Back, EInputState::E_Released);
								}
								else
								{
									PerformInputLogic(EInputType::E_Forward, EInputState::E_Released);
								}
							}

							if (hasReleasedRightAxisInput)
							{
								hasReleasedRightAxisInput = false;

								if (isFacingRight)
								{
									PerformInputLogic(EInputType::E_Forward, EInputState::E_Pressed);
								}
								else
								{
									PerformInputLogic(EInputType::E_Back, EInputState::E_Pressed);
								}
							}
						}
						else if (Value < -0.01f)
						{
							if (!hasReleasedRightAxisInput)
							{
								hasReleasedRightAxisInput = true;
								if (isFacingRight)
								{
									PerformInputLogic(EInputType::E_Forward, EInputState::E_Released);
								}
								else
								{
									PerformInputLogic(EInputType::E_Back, EInputState::E_Released);
								}
							}

							if (hasReleasedLeftAxisInput)
							{
								hasReleasedLeftAxisInput = false;
								if (isFacingRight)
								{
									PerformInputLogic(EInputType::E_Back, EInputState::E_Pressed);
								}
								else
								{
									PerformInputLogic(EInputType::E_Forward, EInputState::E_Pressed);
								}
							}
						}
					}
					else
					{
						if (Value > 0.01f)
						{
							if (!hasReleasedLeftAxisInput)
							{
								hasReleasedLeftAxisInput = true;

								if (isFacingRight)
								{
									PerformInputLogic(EInputType::E_Back, EInputState::E_Released);
								}
								else
								{
									PerformInputLogic(EInputType::E_Forward, EInputState::E_Released);
								}
							}

							if (hasReleasedRightAxisInput)
							{
								hasReleasedRightAxisInput = false;

								if (isFacingRight)
								{
									PerformInputLogic(EInputType::E_Forward, EInputState::E_Pressed);
								}
								else
								{
									PerformInputLogic(EInputType::E_Back, EInputState::E_Pressed);
								}
							}
						}
						else if (Value < -0.01f)
						{
							if (!hasReleasedRightAxisInput)
							{
								hasReleasedRightAxisInput = true;
								if (isFacingRight)
								{
									PerformInputLogic(EInputType::E_Forward, EInputState::E_Released);
								}
								else
								{
									PerformInputLogic(EInputType::E_Back, EInputState::E_Released);
								}
							}

							if (hasReleasedLeftAxisInput)
							{
								hasReleasedLeftAxisInput = false;
								if (isFacingRight)
								{
									PerformInputLogic(EInputType::E_Back, EInputState::E_Pressed);
								}
								else
								{
									PerformInputLogic(EInputType::E_Forward, EInputState::E_Pressed);
								}
							}
						}
					}




                    if (canMove && characterState != ECharacterState::E_Crouching
                        && characterState != ECharacterState::E_Blocking
                        && characterState != ECharacterState::E_NeutralJump
                        && comboState == EComboState::E_None)
                    {                        
                         if (Value > 0.01f)
                         { 
						   hasReleasedRightAxisInput = false;
                           if (!isFacingRight)
                           {
							   if (characterState != ECharacterState::E_DashBack)
							   {
								   characterState = ECharacterState::E_MovingBack;
							   }
							   isPressingBack = true;                            
                           } 
						   else if (characterState != ECharacterState::E_DashForward && characterState != ECharacterState::E_RunForward)
						   {
							   characterState = ECharacterState::E_MovingForward;
						   }
                         }
                         
						 else if (Value < -0.1f)
						 {						
							 hasReleasedLeftAxisInput = false;
							 if (isFacingRight)
							 {
								 if (characterState != ECharacterState::E_DashBack)
								 {
									 characterState = ECharacterState::E_MovingBack;
								 }
								 isPressingBack = false;
							 }
							 else if (characterState != ECharacterState::E_DashForward && characterState != ECharacterState::E_RunForward)
							 {
								 characterState = ECharacterState::E_MovingForward;
							 }
                         }
						 else
						 {
                          characterState = ECharacterState::E_Default;
                          isPressingBack = false;
						 }
						 AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
						
                    }

                     else if (canMove && characterState == ECharacterState::E_Crouching)
                      {
                        if (Value > 0.01f)
                         {
                           if (!isFacingRight)
                           {										
                              isPressingBack = true;
                           }
                        }
                        else if (Value < -0.01f)
                        {

                         if (isFacingRight)
                         {
                           isPressingBack = false;
                         }

                        }
                     }

					 else if (canMove && characterState == ECharacterState::E_NeutralJump	|| characterState == ECharacterState::E_ForwardJump || characterState == ECharacterState::E_BackwardJump|| characterState == ECharacterState::E_WallJump)
					{
						if (Value > 0.01f)
						{
							if (!isFacingRight)
							{
								isPressingBack = true;
							}

						}
						else if (Value < -0.01f)
						{
							if (isFacingRight)
							{
								isPressingBack = true;
							}
						 }
					}
                }				
			}
		}
	}
}
void AFightingGameCharacter::MoveRightPad(float Value)
{
	if (auto baseGameInstance = Cast<UMainGameInstance>(GetGameInstance()) && this)
	{
		if (auto controller = Cast<AFightingGameController>(GetController()))
		{

			if (controller->isInputDeviceGamepad) 
			{
				if (characterState != ECharacterState::E_Crouching)
				{
					if (Value > 0.01f)
					{
						if (!hasReleasedLeftAxisInput)
						{
							hasReleasedLeftAxisInput = true;

							if (isFacingRight)
							{
								PerformInputLogic(EInputType::E_Back, EInputState::E_Released);
							}
							else
							{
								PerformInputLogic(EInputType::E_Forward, EInputState::E_Released);
							}
						}

						if (hasReleasedRightAxisInput)
						{
							hasReleasedRightAxisInput = false;

							if (isFacingRight)
							{
								PerformInputLogic(EInputType::E_Forward, EInputState::E_Pressed);
							}
							else
							{
								PerformInputLogic(EInputType::E_Back, EInputState::E_Pressed);
							}
						}
					}
					else if (Value < -0.01f)
					{
						if (!hasReleasedRightAxisInput)
						{
							hasReleasedRightAxisInput = true;
							if (isFacingRight)
							{
								PerformInputLogic(EInputType::E_Forward, EInputState::E_Released);
							}
							else
							{
								PerformInputLogic(EInputType::E_Back, EInputState::E_Released);
							}
						}

						if (hasReleasedLeftAxisInput)
						{
							hasReleasedLeftAxisInput = false;
							if (isFacingRight)
							{
								PerformInputLogic(EInputType::E_Back, EInputState::E_Pressed);
							}
							else
							{
								PerformInputLogic(EInputType::E_Forward, EInputState::E_Pressed);
							}
						}
					}
				}
				else
				{
					if (Value > 0.01f)
					{
						if (!hasReleasedLeftAxisInput)
						{
							hasReleasedLeftAxisInput = true;

							if (isFacingRight)
							{
								PerformInputLogic(EInputType::E_Back, EInputState::E_Released);
							}
							else
							{
								PerformInputLogic(EInputType::E_Forward, EInputState::E_Released);
							}
						}

						if (hasReleasedRightAxisInput)
						{
							hasReleasedRightAxisInput = false;

							if (isFacingRight)
							{
								PerformInputLogic(EInputType::E_Forward, EInputState::E_Pressed);
							}
							else
							{
								PerformInputLogic(EInputType::E_Back, EInputState::E_Pressed);
							}
						}
					}
					else if (Value < -0.01f)
					{
						if (!hasReleasedRightAxisInput)
						{
							hasReleasedRightAxisInput = true;
							if (isFacingRight)
							{
								PerformInputLogic(EInputType::E_Forward, EInputState::E_Released);
							}
							else
							{
								PerformInputLogic(EInputType::E_Back, EInputState::E_Released);
							}
						}

						if (hasReleasedLeftAxisInput)
						{
							hasReleasedLeftAxisInput = false;
							if (isFacingRight)
							{
								PerformInputLogic(EInputType::E_Back, EInputState::E_Pressed);
							}
							else
							{
								PerformInputLogic(EInputType::E_Forward, EInputState::E_Pressed);
							}
						}
					}
				}
						
				if (canMove && characterState != ECharacterState::E_Crouching
					&& characterState != ECharacterState::E_Blocking
					&& characterState != ECharacterState::E_NeutralJump
					&& comboState == EComboState::E_None)
				{
					if (Value > 0.01f)
					{
						hasReleasedRightAxisInput = false;
						if (!isFacingRight)
						{
							if (characterState != ECharacterState::E_DashBack)
							{
								characterState = ECharacterState::E_MovingBack;
							}
							isPressingBack = true;
						}
						else if (characterState != ECharacterState::E_DashForward && characterState != ECharacterState::E_RunForward)
						{
							characterState = ECharacterState::E_MovingForward;
						}
					}

					else if (Value < -0.1f)
					{
						hasReleasedLeftAxisInput = false;
						if (isFacingRight)
						{
							if (characterState != ECharacterState::E_DashBack)
							{
								characterState = ECharacterState::E_MovingBack;
							}
							isPressingBack = true;
						}
						else if (characterState != ECharacterState::E_DashForward && characterState != ECharacterState::E_RunForward)
						{
							characterState = ECharacterState::E_MovingForward;
						}
					}
					else
					{
						characterState = ECharacterState::E_Default;
						isPressingBack = false;
					}

					AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);

				}

				else if (canMove && characterState == ECharacterState::E_Crouching)
				{
					if (Value > 0.01f)
					{
						if (!isFacingRight)
						{
							isPressingBack = true;
						}
						

					}
					else if (Value < -0.01f)
					{
						if (isFacingRight)
						{					
							isPressingBack = true;
						}
					}

					else
						isPressingBack = false;
				}
				else if (canMove && characterState == ECharacterState::E_NeutralJump
					|| characterState == ECharacterState::E_ForwardJump
					|| characterState == ECharacterState::E_BackwardJump
					|| characterState == ECharacterState::E_WallJump)
				{
					if (Value > 0.01f)
					{
						if (!isFacingRight)
						{
							isPressingBack = true;
						}
					
					}
					else if (Value < -0.01f)
					{
						if (isFacingRight)
						{						
							isPressingBack = true;
						}
					}

				}
			}
			
		}
	}
}

void AFightingGameCharacter::BeginStun()
{
	if(stunFrames > 0)
	{		
		canMove = false;
		canAttack = false;
	}
}

void AFightingGameCharacter::EndStun()
{
	if (comboState != EComboState::E_Launched && comboState != EComboState::E_KnockDown
		&& comboState != EComboState::E_Recovery && comboState != EComboState::E_WallBounce && comboState != EComboState::E_GroundBounce)
	{		
		if (characterState != ECharacterState::E_Crouching && characterState != ECharacterState::E_CrouchBlocking)
		{
				characterState = ECharacterState::E_Default;
				otherPlayer->EndCombo();
		}
		else
		{
				characterState = ECharacterState::E_Crouching;
				otherPlayer->EndCombo();
		}
		
		comboState = EComboState::E_None;
	}
	canMove = true;
	canAttack = true;
}

void AFightingGameCharacter::BeginHitStop(int _hitStopFrames)
{
	if (_hitStopFrames > 0)
	{
		float hitstopModifier = stunFrames / _hitStopFrames;
		
		if (hitstopModifier > 0.0f)
		{
			if (auto gamemode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
			{
				CustomTimeDilation = 0.0f;
				otherPlayer->CustomTimeDilation = 0.0f;
				gamemode->hitStopFrames = _hitStopFrames;
				gamemode->timerActive = false;
			}
		}
		else
		{
			EndHitStop();
		}
	}
	else
	{
		EndHitStop();
	}

}

void AFightingGameCharacter::EndHitStop()
{
	CustomTimeDilation = 1.0f;
	otherPlayer->CustomTimeDilation = 1.0f;
	if (auto gamemode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if(comboState != EComboState::E_INSUPER && otherPlayer->comboState != EComboState::E_INSUPER && !roundLost && !otherPlayer->roundLost)
		{
			gamemode->timerActive = true; 
		}
		
	}
}

void AFightingGameCharacter::FGTakeDamage(float Damage, int _stunFrames, int _blockStunFrames, int _hitStopFrames,
	float _pushbackAmount, float _launchAmount, bool _shouldIgnoreStunDecay, bool _shouldIgnoreDamageDecay)
{
	
	bool isDefeatedonHit = false;
	if (comboState != EComboState::E_INSUPER && !takenDamageThisFrame)
	{
		
		if (characterState != ECharacterState::E_Blocking && characterState != ECharacterState::E_CrouchBlocking)
		{			
			if (stunFrames > 0 || otherPlayer->comboCounter == 0) {
				otherPlayer->comboCounter++;
				otherPlayer->BeginCombo();
				otherPlayer->DetermineComboRating();
			}

			if (!_shouldIgnoreDamageDecay && otherPlayer) {
				float decayedDamage = Damage - (otherPlayer->comboCounter * damageDecayModifier) * .01f;
				if (decayedDamage < .01f) {
					decayedDamage = .01f;
				}
				playerHealth -= decayedDamage;
				takenDamageThisFrame = true;
				damageDecayModifier += .25f;
				takenDamageThisFrame = true;
				lastDamageReceived = decayedDamage;
			}
			else 
			{
				lastDamageReceived = Damage * damageDecayModifier;
				playerHealth -= lastDamageReceived;
				
			}
			

			superMeterAmount += 0.15f;



			if (stunFrames > 0 && !_shouldIgnoreDamageDecay && otherPlayer) {
				
				stunFrames = _stunFrames - otherPlayer->comboCounter * hitStunDecayModifier;				
				hitStunDecayModifier += 0.5f;				
			}
			else {
				stunFrames = _stunFrames;				
			}

			if (characterState == ECharacterState::E_Default)
			{
				comboState = EComboState::E_StandingStun;
			}
			else if (characterState == ECharacterState::E_Crouching)
			{
				comboState = EComboState::E_CrouchStun;
			}
			else if (characterState == ECharacterState::E_NeutralJump || characterState == ECharacterState::E_ForwardJump || characterState == ECharacterState::E_BackwardJump)
			{
				comboState = EComboState::E_Launched;
			}
			BeginStun();

			BeginHitStop(_hitStopFrames);
			PlayDamageSoundEffect();

			if (playerHealth <= 0.0f)
				otherPlayer->WinRound();
			PerformPushBack(_pushbackAmount, _launchAmount, false);
		}

		else
		{
			bool performIB = false;
			float reducedDamage = Damage * 0.1f;
			int maxFrameForIB = startInstantBlockFrame + instantBlockWindow;

			if (maxFrameForIB > 59)
			{
				maxFrameForIB -= 60;
				//Uses the currentTick system from the Tick function. Perhaps we can use GFrameCounter instead to remove need to check an edge case
				if (startInstantBlockFrame > -1 && currentTick >= startInstantBlockFrame
					|| currentTick <= maxFrameForIB)
				{
					performIB = true;
					reducedDamage = 0.f;
				}
			}
			else
			{
				//Uses the currentTick system from the Tick function. Perhaps we can use GFrameCounter instead to remove need to check an edge case
				if (startInstantBlockFrame > -1 && currentTick >= startInstantBlockFrame
					&& currentTick <= maxFrameForIB)
				{
					performIB = true;
					reducedDamage = 0.f;

				}
			}			
			playerHealth -= reducedDamage;
			superMeterAmount += 0.15f;
			stunFrames = _blockStunFrames;
			BeginStun();
			BeginHitStop(_hitStopFrames);
			if (!performIB)
			{
				PerformPushBack(_pushbackAmount, 0.0f, true);

			}
		}


		if (otherPlayer)
		{
			if (otherPlayer->lastCommandUsed.hasAutoCombo)
			{
				otherPlayer->gattlingLevel++;
			}
			otherPlayer->hasLandedHit = true;
			otherPlayer->PerformPushBack(_pushbackAmount, 0.0f, false);
			if (otherPlayer->wasExAttackUsed != true)
			{
				otherPlayer->superMeterAmount += Damage * 0.2f;
			}
		}

		if (playerHealth <= 0.0f)
		{
			playerHealth = 0.0f;
			isDefeatedonHit = true;
		}
	}
	else
	{
		playerHealth -= Damage;
		if (playerHealth < 0.00f)
		{
			isDefeatedonHit = true;
		}
	}

	if (isDefeatedonHit)
	{
		playerHealth = 0.0f;
		if (auto gameMode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
		{
			gameMode->RoundWon(otherPlayer);
		}
	}

	
}

void AFightingGameCharacter::SetHealth(float _health)
{
	playerHealth = _health;
}

float AFightingGameCharacter::GetMaxHealth()
{
	return maxPlayerHealth;
}

void AFightingGameCharacter::SetSuperMeter(float _meter)
{
	superMeterAmount = _meter;
}

float AFightingGameCharacter::GetMaxSuperMeter()
{
	return maxSuperMeterAmount;
}

void AFightingGameCharacter::WinMatch()
{
	canMove = false;
	canTakeDamage = false;
	wonMatch = true;
}

void AFightingGameCharacter::CustomLaunchPlayer(FVector _LaunchVelocity, bool _shouldOverrideXY, bool _shouldOverrideZ, bool _ShouldIgnorePlayer)
{
	if (_ShouldIgnorePlayer)
	{
		IgnoreCollision(true);
	}
	LaunchCharacter(_LaunchVelocity, _shouldOverrideXY, _shouldOverrideZ);
}

void AFightingGameCharacter::WinRound()
{
	if(!otherPlayer->roundLost)
	{	
		otherPlayer->roundLost = true;
		++roundsWon;
	}
}

void AFightingGameCharacter::addInputToInputbuffer(FInputInfo _inputInfo)
{
	if (!isFacingRight)
	{
		if (_inputInfo.inputType == EInputType::E_Forward)
		{
			_inputInfo.inputType = EInputType::E_Back;
		}
		else if (_inputInfo.inputType == EInputType::E_Back)
		{
			_inputInfo.inputType = EInputType::E_Forward;
		}
	}

	if (!capturedInputThisFrame)
	{
		inputBuffer[currentTick].inputs[0] = _inputInfo;
		capturedInputThisFrame = true;
	}
	else
	{
		++currentFrameInputIndex;
		inputBuffer[currentTick].inputs[currentFrameInputIndex] = _inputInfo;
	}	
	CheckInputBufferForCommand(inputBuffer[currentTick].inputs[currentFrameInputIndex].inputState);	
}

void AFightingGameCharacter::CheckInputBufferForCommand(EInputState _inputState)
{
	int correctSequenceCounter = 0;
	
    
        for (int currentCommand = 0; currentCommand < commandList.Num(); ++currentCommand)
        {
            if (superMeterAmount >= commandList[currentCommand].meterCost)
            {
                correctSequenceCounter = commandList[currentCommand].inputTypes.Num() - 1;

                for (int frame = 0; frame < commandList[currentCommand].maxInputWindow; ++frame)
                {
                    int frameDataToCheck = (currentTick - frame + inputBuffer.Capacity()) % inputBuffer.Capacity();
                    for (int i = 0; i < maxInputsPerFrame; ++i)
                    {
                        if (inputBuffer[frameDataToCheck].inputs[i].inputType != EInputType::E_None)
                        {
                            EInputType currentInputType = inputBuffer[frameDataToCheck].inputs[i].inputType;
                            EInputState currentInputState = inputBuffer[frameDataToCheck].inputs[i].inputState;
                            int64 chargedFrames = inputBuffer[frameDataToCheck].inputs[i].chargeFrame;

                            if (chargedFrames > 0)
                            {
                                chargedFrames = chargedFrames - 1;
                            }
                            if (currentInputType == commandList[currentCommand].inputTypes[commandList[currentCommand].inputTypes.Num() - 1].inputType
                                && _inputState == EInputState::E_Released && commandList[currentCommand].isCharging)
                            {
                                commandList[currentCommand].isCharging = false;
                                commandList[currentCommand].currentFramesHeld = 0;
                            }


                            if (correctSequenceCounter > -1)
                            {
                                if ((IsInputMultiInputCommand(commandList[currentCommand], currentInputType) && currentInputState != EInputState::E_Released || currentInputType == commandList[currentCommand].inputTypes[correctSequenceCounter].inputType)
                                    && (currentInputState == commandList[currentCommand].inputTypes[correctSequenceCounter].inputState || currentInputState == EInputState::E_Pressed && commandList[currentCommand].inputTypes[correctSequenceCounter].inputState == EInputState::E_Held)
                                    && chargedFrames >= commandList[currentCommand].inputTypes[correctSequenceCounter].requiredChargeFrames /*End of IF*/)
                                {
                                    --correctSequenceCounter;
                                }
                                else if (currentInputType != EInputType::E_None && currentInputState != EInputState::E_Released)
                                {
                                    correctSequenceCounter = commandList[currentCommand].inputTypes.Num() - 1;
                                }
                                else if (IsInputMultiInputCommand(commandList[currentCommand], currentInputType) && currentInputState == EInputState::E_Released)
                                {
                                    correctSequenceCounter = commandList[currentCommand].inputTypes.Num() - 1;
                                    for (int j = 0; j < commandList[currentCommand].inputTypes.Num(); ++j)
                                    {
                                        if (commandList[currentCommand].inputTypes[j].inputType == currentInputType)
                                        {
                                            commandList[currentCommand].inputTypes[j].isCurrentlyHeld = false;
                                        }
                                    }
                                }
                            }
                            if (correctSequenceCounter == -1 && _inputState != EInputState::E_Released && characterState == commandList[currentCommand].requiredState)
                            {
                                moveBuffer.Add(commandList[currentCommand]);
                                break;
                            }

                        }
                        else
                        {
                            break;
                        }

                    }
                }
            }

        }
	
	

}

bool AFightingGameCharacter::IsInputMultiInputCommand(FCommand _command, EInputType _pressedInput)
{
	for (int i = _command.inputTypes.Num() -1; i >=0 ; --i)
	{
		if (_command.inputTypes[i].inputType == _pressedInput && _command.inputTypes[i].inputState == EInputState::E_Held)
		{
			_command.inputTypes[i].isCurrentlyHeld = true;
		}
	}
	return false;
}

void AFightingGameCharacter::StartTrackingChargeTime(FChargeInput _trackedInput)
{
	for (int i = 0; i < ChargeTimes.Num(); ++i)
	{
		if(ChargeTimes[i].inputType == _trackedInput.inputType)
		{
			ChargeTimes[i].IsHoldingInput = true;
		}
	}
}

void AFightingGameCharacter::ResetChargeTime(FChargeInput _trackedInput)
{
	for (int i = 0; i < ChargeTimes.Num(); ++i)
	{
		if (ChargeTimes[i].inputType == _trackedInput.inputType)
		{
			ChargeTimes[i].IsHoldingInput =false;
			ChargeTimes[i].ChargeFrames = 0;
		}
	}
}

void AFightingGameCharacter::DetermineCommandToUse()
{
	if (moveBuffer.Num() > 0)
	{
		FCommand moveToUse = moveBuffer[0];
		if (hasLandedHit)
		{
			StartCommand(moveToUse.name);
		}
		else
		{
			for (int i = 0; i < moveBuffer.Num(); ++i)
			{
				if (moveToUse.inputTypes.Num() < moveBuffer[i].inputTypes.Num() && moveToUse.commandPriority < moveBuffer[i].commandPriority)
				{
					moveToUse = moveBuffer[i];
				}
			}
			if (attackState == EAttackState::E_NONE || attackState == EAttackState::E_ATTACKRECOVERY)
			{
				StartCommand(moveToUse.name);
			}
		}
	
		moveBuffer.Empty();

	}

	
}

void AFightingGameCharacter::StartCommand(FString _commandName)
{

	for (int currentCommand = 0; currentCommand < commandList.Num(); ++currentCommand)
	{
		if (_commandName.Compare(commandList[currentCommand].name) == 0)
		{			
			if (canAttack)
			{
				if (superMeterAmount >= commandList[currentCommand].meterCost)
				{
					superMeterAmount -= commandList[currentCommand].meterCost;
					if (!commandList[currentCommand].isCharging && commandList[currentCommand].maxFramesHeld > 0)
					{
						commandList[currentCommand].isCharging = true;
					}
					//if(finishTransition &&  ChangeState(commandList[currentCommand].stateID))
					//{
					//	
					//	//commandList[currentCommand].hasUsedCommand = true;
					//	UE_LOG(LogTemp, Warning, TEXT("Executing Command with state ID: %d"), commandList[currentCommand].stateID);
					//	break;
					//}
					

										
					if (commandList[currentCommand].resulatingState != ECharacterState::E_Default)
					{
						lastCommandUsed = commandList[currentCommand];
						characterState = commandList[currentCommand].resulatingState;
					}
				}
			}			
		}
	}
	
}


void AFightingGameCharacter::AddtoInputMap(FString _input, EInputType _type)
{
	//inputType.Add(_input, _type);
}

void AFightingGameCharacter::StartLightAttack()
{
	PerformInputLogic(EInputType::E_LightAttack, EInputState::E_Pressed);
}

void AFightingGameCharacter::StartMediumAttack()
{
	PerformInputLogic(EInputType::E_MediumAttack, EInputState::E_Pressed);
}

void AFightingGameCharacter::StartHeavyAttack()
{
	PerformInputLogic(EInputType::E_HeavyAttack, EInputState::E_Pressed);
}

void AFightingGameCharacter::StartSpecialAttack()
{
	PerformInputLogic(EInputType::E_SpecialAttack, EInputState::E_Pressed);
}

void AFightingGameCharacter::StartThrow()
{
	PerformInputLogic(EInputType::E_Throw, EInputState::E_Pressed);
}

void AFightingGameCharacter::PressMacro1()
{
	if (auto gameInstance = Cast<UMainGameInstance>(GetGameInstance()))
	{
		for (int i = 0; i < gameInstance->P1Macros.Num(); ++i)
		{
			PerformInputLogic(gameInstance->P1Macros[i], EInputState::E_Pressed);
		}
	}
}

void AFightingGameCharacter::ReleaseLightAttack()
{
	PerformInputLogic(EInputType::E_LightAttack, EInputState::E_Released);
}

void AFightingGameCharacter::ReleaseMediumAttack()
{
	PerformInputLogic(EInputType::E_MediumAttack, EInputState::E_Released);
}

void AFightingGameCharacter::ReleaseHeavyAttack()
{
	PerformInputLogic(EInputType::E_HeavyAttack, EInputState::E_Released);
}

void AFightingGameCharacter::ReleaseSpecialAttack()
{
	PerformInputLogic(EInputType::E_SpecialAttack, EInputState::E_Released);
}

void AFightingGameCharacter::ReleaseMacro1()
{
	if (auto gameInstance = Cast<UMainGameInstance>(GetGameInstance()))
	{
		for (int i = 0; i < gameInstance->P1Macros.Num(); ++i)
		{
			PerformInputLogic(gameInstance->P1Macros[i], EInputState::E_Released);
		}
	}

}

void AFightingGameCharacter::Jump()
{
	PerformInputLogic(EInputType::E_Up, EInputState::E_Pressed);
	if (canMove && comboState == EComboState::E_None
		&& characterState != ECharacterState::E_Crouching
		&& jumpCount < maxJumpCount)
	{
		//IgnoreCollision(true);
		if (characterState == ECharacterState::E_MovingBack)
		{
			if (isFacingRight)
			{
				//Launch Player Left and Up
				CustomLaunchPlayer(FVector(-jumpDistance, 0.0f, jumpHeight), true, true, false);
				characterState = ECharacterState::E_BackwardJump;
			}
			else
			{
				//Launch Player Right and Up
				CustomLaunchPlayer(FVector(jumpDistance, 0.0f, jumpHeight), true, true, false);
				characterState = ECharacterState::E_ForwardJump;
			}
			
		}
		else if (characterState == ECharacterState::E_MovingForward)
		{
			if (!isFacingRight)
			{
				//Launch Player Left and Up
				CustomLaunchPlayer(FVector(-jumpDistance, 0.0f, jumpHeight), true, true, false);
				characterState = ECharacterState::E_ForwardJump;
			}
			else
			{
				//Launch Player Right and Up
				CustomLaunchPlayer(FVector(jumpDistance, 0.0f, jumpHeight), true, true, false);
				characterState = ECharacterState::E_BackwardJump;
			}
		} 
		else
		{
			//Launch Player Up
			CustomLaunchPlayer(FVector(0.0f, 0.0f, jumpHeight), false, true, false);
			//Set Character State To Jumping
			characterState = ECharacterState::E_NeutralJump;
		}
		//Increment Jump Count
		++jumpCount;

	}
	else if (comboState == EComboState::E_KnockDown)
	{
		comboState = EComboState::E_Recovery;
	}
}

void AFightingGameCharacter::StopJumping()
{ 
	ACharacter::StopJumping();
}

void AFightingGameCharacter::Landed(const FHitResult& Hit)
{
	GetMovementComponent()->StopMovementImmediately();
	if (characterState == ECharacterState::E_NeutralJump || characterState == ECharacterState::E_ForwardJump || characterState == ECharacterState::E_BackwardJump || characterState == ECharacterState::E_WallJump)
	{
		CheckStuckInOtherCharacter();
		//if (isStuckInEnemy)
		//{
		//	float distanceApart = fabs(GetActorLocation().Y - otherPlayer->GetActorLocation().Y);

		//	float distanceToMove = 145.0f;

		//	UE_LOG(LogTemp, Warning, TEXT("Landed on Other Player!!!"));

		//	if (otherPlayer->isFacingRight) {
		//		
		//		MoveCharacterSmoothly(GetActorLocation(), FVector(GetActorLocation().X + ((distanceToMove - distanceApart) * 0.5f), GetActorLocation().Y, GetActorLocation().Z - 30.0f));
		//		otherPlayer->MoveCharacterSmoothly(otherPlayer->GetActorLocation(), FVector(otherPlayer->GetActorLocation().X + ((distanceToMove - distanceApart) * 0.5f) * -1.0f, otherPlayer->GetActorLocation().Y , otherPlayer->GetActorLocation().Z - 30.0f));
		//	}
		//	else {
		//		MoveCharacterSmoothly(GetActorLocation(), FVector(GetActorLocation().X + ((distanceToMove - distanceApart) * 0.5f), GetActorLocation().Y, GetActorLocation().Z + 30.0f));
		//		otherPlayer->MoveCharacterSmoothly(otherPlayer->GetActorLocation(), FVector(otherPlayer->GetActorLocation().X + ((distanceToMove - distanceApart) * 0.5f), otherPlayer->GetActorLocation().Y , otherPlayer->GetActorLocation().Z - 30.0f));

		//	}			
		//	GetCharacterMovement()->GravityScale = gravityScale;
		//	gravityscaleModifier = 0.7f;
		//}
		if (!Cast<AHitboxActor>(Hit.GetActor()))
		{
			GetCharacterMovement()->GravityScale = gravityScale;
			gravityscaleModifier = 0.7f;
			characterState = ECharacterState::E_Default;
			comboState = EComboState::E_None;
		}
		jumpCount = 0;
		IgnoreCollision(false);
		
	}
	else if ((comboState == EComboState::E_Launched && !shouldGroundBounce) || comboState == EComboState::E_WallBounce || comboState == EComboState::E_GroundBounce)
	{
		if (!Cast<AHitboxActor>(Hit.GetActor()))
		{
			GetCharacterMovement()->GravityScale = gravityScale;
			gravityscaleModifier = 0.7f;
			if (comboState == EComboState::E_WallBounce || comboState == EComboState::E_GroundBounce)
			{
				CheckStuckInOtherCharacter();
			}
			if (!shouldHardKnockDown)
			{
				comboState = EComboState::E_KnockDown;
			}
			else
			{
				shouldHardKnockDown = false;
			}
		}

		
		otherPlayer->EndCombo();
		jumpCount = 0;
		IgnoreCollision(false);
	}
	else if (shouldGroundBounce) {
		comboState = EComboState::E_GroundBounce;
		jumpCount = 0;
		IgnoreCollision(true);
	}
}

void AFightingGameCharacter::StartCrouching()
{

	PerformInputLogic(EInputType::E_Down, EInputState::E_Pressed);	

	if (canMove && comboState != EComboState::E_StandingStun && characterState != ECharacterState::E_NeutralJump
		&& characterState != ECharacterState::E_ForwardJump && characterState != ECharacterState::E_BackwardJump
		&& comboState != EComboState::E_Launched && comboState != EComboState::E_Recovery
		&& comboState != EComboState::E_GroundBounce
		&& comboState != EComboState::E_WallBounce)
	{
		characterState = ECharacterState::E_Crouching;
		isCrouching = true;
	}

}

void AFightingGameCharacter::StopCrouching()
{
	PerformInputLogic(EInputType::E_Down, EInputState::E_Released);

	if (comboState != EComboState::E_StandingStun && characterState != ECharacterState::E_NeutralJump
		&& characterState != ECharacterState::E_ForwardJump && characterState != ECharacterState::E_BackwardJump
		&& comboState != EComboState::E_Launched && comboState != EComboState::E_Recovery
		&& comboState != EComboState::E_GroundBounce
		&& comboState != EComboState::E_WallBounce)
	{
		characterState = ECharacterState::E_Default;
		isCrouching = false;
	}
}

void AFightingGameCharacter::StartBlocking()
{
	if (canMove && comboState == EComboState::E_None)
	{

		if (isCrouching)
		{
			characterState = ECharacterState::E_CrouchBlocking;
		}
		else
		{
			characterState = ECharacterState::E_Blocking;			
		}
		//Track block attempts for Instant Block spam protection
		if (numFramesUntilInstantBlockAttempt <= 0)
		{
			startInstantBlockFrame = currentTick;			
		}
		else
		{		
			startInstantBlockFrame = -1;
		}
		numFramesUntilInstantBlockAttempt = framesBetweenInstantBlockAttempt;
	}
	
}

void AFightingGameCharacter::StopBlocking()
{
	if (canMove && comboState != EComboState::E_Launched && comboState != EComboState::E_StandingStun && comboState != EComboState::E_CrouchStun)
	{
		if (isCrouching)
		{
			characterState = ECharacterState::E_Crouching;
		}
		else
		{
			characterState = ECharacterState::E_Default;
		}
	}
	
}

void AFightingGameCharacter::CollidedWithProximityBox(int _blockstunFrames)
{
	if (isPressingBack)
	{		
		StartBlocking();
		stunFrames = _blockstunFrames;
		BeginStun();
	}
}

void AFightingGameCharacter::CheckStuckInOtherCharacter()
{
	TArray<AActor*> overlappingActors;
	GetOverlappingActors(overlappingActors, AFightingGameCharacter::StaticClass());

	for (int i = 0; i < overlappingActors.Num(); ++i)
	{
		if (auto overlappingCharacter = Cast<AFightingGameCharacter>(overlappingActors[i]))
		{
			isStuckInEnemy = true;
			overlappingCharacter->isStuckInEnemy = true;
		}
	}
}

void AFightingGameCharacter::BeginCombo()
{
	if (otherPlayer->shouldGroundBounce || otherPlayer->shouldWallBounce)
	{
		hasUsedbounce = true;
	}
	/*Ends the combo if a combo ender is used or a secondary bounce move was used*/
	if (lastCommandUsed.comboType == EComboType::E_ComboEnder || hasUsedbounce)
	{
		EndCombo();
	}
	
	

}

void AFightingGameCharacter::EndCombo()
{
	otherPlayer->hitStunDecayModifier = 2.0f;
	otherPlayer->damageDecayModifier = 1.5f;
	comboCounter = 0;
	lastComboRating = EComboRating::E_Default;
	/*Ill advised to do so. Should clear the last command. Currently set it to the standing light attack.*/
	//lastCommandUsed = commandList[0];

}

void AFightingGameCharacter::DetermineComboRating()
{
	if (comboCounter > 9)
	{
		lastComboRating = EComboRating::E_Ultimate;
	}
	else if (comboCounter > 7)
	{
		lastComboRating = EComboRating::E_Awesome;
	}

	else if (comboCounter > 5)
	{
		lastComboRating = EComboRating::E_Execel;
	}

	else if (comboCounter > 3)
	{
		lastComboRating = EComboRating::E_Great;
	}

	else if (comboCounter > 1)
	{
		lastComboRating = EComboRating::E_Good;
	}

	else { lastComboRating = EComboRating::E_Default; }
}

void AFightingGameCharacter::PerformPushBack(float _pushbackAmount, float _launchAmount, bool _isBlocking)
{
	if (_isBlocking)
	{
		if (isFlipped)
		{
			CustomLaunchPlayer(FVector(-_pushbackAmount * 2.0f, 0.0f, 0.0f), false, false, false);
		}
		else
		{
			CustomLaunchPlayer(FVector(_pushbackAmount*2.0f, 0.0f, 0.0f), false, false, false);
		}
	}
	else
	{
		if (_launchAmount > 0.0f)
		{
			GetCharacterMovement()->GravityScale = gravityScale * gravityscaleModifier;
			gravityscaleModifier += 0.1f;
			comboState = EComboState::E_Launched;
		}

		if (isFacingRight)
		{				
			CustomLaunchPlayer(FVector(-_pushbackAmount, 0.0f, _launchAmount), false, false, false);
		}
		else
		{
			CustomLaunchPlayer(FVector(_pushbackAmount, 0.0f, _launchAmount), false, false, false);
		}
		

	}
}
