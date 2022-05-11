// Copyright Epic Games, Inc. All Rights Reserved.

#include "FightingGameCharacter.h"
#include "FightingGameGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

//////////////////////////////////////////////////////////////////////////
// AFightingGameCharacter

AFightingGameCharacter::AFightingGameCharacter()
{
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


	playerHealth = 1.0f;
	wasLightAttackUsed = false;
	wasMediumAttackUsed = false;
	wasHeavyAttackUsed = false;
	wasSpecialAttackUsed = false;
	wasExAttackUsed = false;
	wasSuperAttackUsed = false;
	hasLandedHit = false;
	superMeterAmount = 1.0f;
	Hurtbox = nullptr;
	otherPlayer = nullptr;
	isFlipped = false;
	scale = FVector(0.0f, 0.0f, 0.0f);
	transform = GetActorTransform();
	characterState = ECharacterState::VE_Default;
	isCrouching = false;
	MaxDistanceApart = 800.0f;
	gravityScale = GetCharacterMovement()->GravityScale;
	stunTime = 0.0f;
	canMove = true;
	inputDecayTime = 5.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFightingGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (auto gameMode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (gameMode->player1 == this)
		{
			// Set up gameplay key bindings
			check(PlayerInputComponent);
			PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFightingGameCharacter::Jump);
			PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFightingGameCharacter::StopJumping);
			PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFightingGameCharacter::StartCrouching);
			PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFightingGameCharacter::StopCrouching);
			PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AFightingGameCharacter::StartBlocking);
			PlayerInputComponent->BindAction("Block", IE_Released, this, &AFightingGameCharacter::StopBlocking);
			PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AFightingGameCharacter::StartLightAttack);
			//PlayerInputComponent->BindAction("LightAttack", IE_Released, this, &AFightingGameCharacter::StartLightAttack);
			PlayerInputComponent->BindAction("MediumAttack", IE_Pressed, this, &AFightingGameCharacter::StartMediumAttack);
			//PlayerInputComponent->BindAction("MediumAttack", IE_Released, this, &AFightingGameCharacter::StartMediumAttack);
			PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AFightingGameCharacter::StartHeavyAttack);
			//PlayerInputComponent->BindAction("HeavyAttack", IE_Released, this, &AFightingGameCharacter::StartHeavyAttack);
			PlayerInputComponent->BindAction("SpecialAttack", IE_Pressed, this, &AFightingGameCharacter::StartSpecialAttack);
			//PlayerInputComponent->BindAction("SpecialAttack", IE_Released, this, &AFightingGameCharacter::StartSpecialAttack);
			PlayerInputComponent->BindAction("ExAttack", IE_Pressed, this, &AFightingGameCharacter::StartExAttack);
			PlayerInputComponent->BindAxis("Move Right", this, &AFightingGameCharacter::MoveRight);
		}
		else
		{
			PlayerInputComponent->BindAction("P2Jump", IE_Pressed, this, &AFightingGameCharacter::Jump);
			PlayerInputComponent->BindAction("P2Jump", IE_Released, this, &AFightingGameCharacter::StopJumping);
			PlayerInputComponent->BindAction("P2Crouch", IE_Pressed, this, &AFightingGameCharacter::StartCrouching);
			PlayerInputComponent->BindAction("P2Crouch", IE_Released, this, &AFightingGameCharacter::StopCrouching);
			PlayerInputComponent->BindAction("P2Block", IE_Pressed, this, &AFightingGameCharacter::StartBlocking);
			PlayerInputComponent->BindAction("P2Block", IE_Released, this, &AFightingGameCharacter::StopBlocking);
			PlayerInputComponent->BindAction("P2LightAttack", IE_Pressed, this, &AFightingGameCharacter::StartLightAttack);
			PlayerInputComponent->BindAction("P2MediumAttack", IE_Pressed, this, &AFightingGameCharacter::StartMediumAttack);
			PlayerInputComponent->BindAction("P2HeavyAttack", IE_Pressed, this, &AFightingGameCharacter::StartHeavyAttack);
			PlayerInputComponent->BindAction("P2SpecialAttack", IE_Pressed, this, &AFightingGameCharacter::StartSpecialAttack);
			PlayerInputComponent->BindAction("P2ExAttack", IE_Pressed, this, &AFightingGameCharacter::StartExAttack);
			PlayerInputComponent->BindAxis("P2MoveRight", this, &AFightingGameCharacter::P2MoveRight);
		}
	}
}


void AFightingGameCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && /*(Value != 0.0f)  &&*/ canMove && characterState != ECharacterState::VE_Blocking && characterState != ECharacterState::VE_Crouching && characterState != ECharacterState::VE_Jumping
		&& characterState != ECharacterState::VE_Launched)
	{		
			if (Value > 0.2f)
			{
				characterState = ECharacterState::VE_MovingRight;
			}
			else if (Value < -0.2f)
			{
				characterState = ECharacterState::VE_MovingLeft;
			}
			else
			{
				characterState = ECharacterState::VE_Default;
			}
	
		float currentDistanceApart = abs(otherPlayer->GetActorLocation().X - GetActorLocation().X);
		
		if (currentDistanceApart >= MaxDistanceApart)
		{
			if ((currentDistanceApart + Value < currentDistanceApart && !isFlipped) || (currentDistanceApart - Value < currentDistanceApart && isFlipped))
			{
				AddMovementInput(FVector(1.0f,0.0f,0.0f), Value);
			}
		}
		else
		{
			AddMovementInput(FVector(1.0f,0.0f,0.0f), Value);
		}
	}
	//if ( (Controller != nullptr) && (Value != 0.0f) )
	//{
	//	// find out which way is right
	//	const FRotator Rotation = Controller->GetControlRotation();
	//	const FRotator YawRotation(0, Rotation.Yaw, 0);
	//
	//	// get right vector 
	//	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//	// add movement in that direction
	//	
	//}
}

void AFightingGameCharacter::FGTakeDamage(float Damage, float _stunTime, float _blockStunTime, float _pushbackAmount, float _launchAmount)
{
	if (characterState != ECharacterState::VE_Blocking)
	{	
		UE_LOG(LogTemp, Warning, TEXT("we are taking %f damage"), Damage);
		superMeterAmount += 0.15f;
		
		stunTime = _stunTime;
		
		if (stunTime > 0.0f)
		{
			characterState = ECharacterState::VE_Stunned;
			BeginStun();
		}
		else
		{
			characterState = ECharacterState::VE_Default;
		}

		playerHealth -= Damage;
		PerformPushBack(_pushbackAmount, _launchAmount, false);
	}

	else
	{
		float reducedDamage = Damage * 0.1f;
		playerHealth -= reducedDamage;
		superMeterAmount += 0.15f;
		UE_LOG(LogTemp, Warning, TEXT("we are taking %f damage"), reducedDamage);

		stunTime = _blockStunTime;
		if (stunTime > 0.0f)
		{
			BeginStun();
		}
		else
			characterState = ECharacterState::VE_Default;
		PerformPushBack(_pushbackAmount, 0.0f, true);
	}


	if (otherPlayer)
	{
		otherPlayer->hasLandedHit = true;
		otherPlayer->PerformPushBack(_pushbackAmount, 0.0f, false);
		if (otherPlayer->wasExAttackUsed != true)
		{
			otherPlayer->superMeterAmount += Damage * 0.2f;
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("The other player is not valid in this context"));
	
	if (playerHealth < 0.0f)
		playerHealth = 0.0f;
}

void AFightingGameCharacter::addInputToInputbuffer(FInputInfo _inputInfo)
{
	inputBuffer.Add(_inputInfo);
	//GetWorld()->GetTimerManager().SetTimer(inputBufferHandle, this, &AFightingGameCharacter::removeInputFromBuffer, inputDecayTime, false);
}

void AFightingGameCharacter::removeInputFromBuffer()
{
}

void AFightingGameCharacter::StartLightAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Light Attack Used"));
	wasLightAttackUsed = true;
}

void AFightingGameCharacter::StartMediumAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Medium Attack Used"));
	wasMediumAttackUsed = true;
}

void AFightingGameCharacter::StartHeavyAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Heavy Attack Used"));
	wasHeavyAttackUsed = true;
}

void AFightingGameCharacter::StartSpecialAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Special Attack Used"));
	wasSpecialAttackUsed = true;	
}

void AFightingGameCharacter::StartExAttack()
{
	wasSuperAttackUsed = true;
	//if (superMeterAmount >= 0.5f)
	//{
	//	superMeterAmount -= 0.5f;
	//	wasExAttackUsed = true;
	//}
	//else if (superMeterAmount >= 1.0f)
	//{
	//	wasSuperAttackUsed = true;
	//	superMeterAmount -= 1.0f;
	//}

}

void AFightingGameCharacter::P2StartLightAttack()
{
	StartLightAttack();
}

void AFightingGameCharacter::P2StartMediumAttack()
{
	StartMediumAttack();
}

void AFightingGameCharacter::P2StartHeavyAttack()
{
	StartHeavyAttack();
}

void AFightingGameCharacter::P2StartSpecialAttack()
{
	StartSpecialAttack();
}

void AFightingGameCharacter::P2Jump()
{
	Jump();
}

void AFightingGameCharacter::P2StopJump()
{
	StopJumping();
}

void AFightingGameCharacter::P2StartExAttack()
{
	StartExAttack();
}

void AFightingGameCharacter::P2MoveRight(float _value)
{
	if(auto gameMode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
		if (gameMode->player2 == this)
		{			
				MoveRight(_value);
		}
	
}

void AFightingGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(characterState != ECharacterState::VE_Jumping)
	{
		if (otherPlayer)
		{
			if (auto characterMovement = GetCharacterMovement())
			{
				if (auto enemyMovement = otherPlayer->GetCharacterMovement())
				{
					if (enemyMovement->GetActorLocation().X >= characterMovement->GetActorLocation().X)
					{
						if (!isFlipped)
						{
							if (auto mesh = GetCapsuleComponent()->GetChildComponent(1))
							{
								transform = mesh->GetRelativeTransform();
								scale = transform.GetScale3D();
								scale.Y = 1;
								scale.X = 1;
								transform.SetScale3D(scale);
								mesh->SetRelativeTransform(transform);

							}
							isFlipped = true;
						}
					}
					else
					{
						if (isFlipped)
						{
							if (auto mesh = GetCapsuleComponent()->GetChildComponent(1))
							{
								transform = mesh->GetRelativeTransform();
								scale = transform.GetScale3D();
								scale.X = -1;
								scale.Y = -1;
								transform.SetScale3D(scale);
								mesh->SetRelativeTransform(transform);

							}
							isFlipped = false;
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

void AFightingGameCharacter::Jump()
{
	ACharacter::Jump();
	characterState = ECharacterState::VE_Jumping;
}

void AFightingGameCharacter::StopJumping()
{ 
	ACharacter::StopJumping();
}

void AFightingGameCharacter::Landed(const FHitResult& Hit)
{
	if (characterState == ECharacterState::VE_Launched || characterState == ECharacterState::VE_Jumping)
	{
		GetCharacterMovement()->GravityScale = gravityScale;
		characterState = ECharacterState::VE_Default;
	}
}

void AFightingGameCharacter::BeginStun()
{
	canMove = false;
	GetWorld()->GetTimerManager().SetTimer(stunHandle, this, &AFightingGameCharacter::EndStun, stunTime, false);
}

void AFightingGameCharacter::EndStun()
{	
	canMove = true;
	if (characterState != ECharacterState::VE_Launched) {
		characterState = ECharacterState::VE_Default;
	}
}

void AFightingGameCharacter::StartCrouching()
{
	characterState = ECharacterState::VE_Crouching;
}

void AFightingGameCharacter::StopCrouching()
{
	characterState = ECharacterState::VE_Default;
}

void AFightingGameCharacter::StartBlocking()
{
	characterState = ECharacterState::VE_Blocking;
}

void AFightingGameCharacter::StopBlocking()
{
	characterState = ECharacterState::VE_Default;
}

void AFightingGameCharacter::CollidedWithProximityBox()
{
	if ((characterState == ECharacterState::VE_MovingLeft && !isFlipped) || (characterState == ECharacterState::VE_MovingRight && isFlipped))
	{
		characterState = ECharacterState::VE_Blocking;
		UE_LOG(LogTemp, Warning, TEXT("Character is Autoblocking!"));
	}
}

void AFightingGameCharacter::PerformPushBack(float _pushbackAmount, float _launchAmount, bool _isBlocking)
{
	if (_isBlocking)
	{
		if (isFlipped)
		{
			LaunchCharacter(FVector(-_pushbackAmount * 2.0f, 0.0f, 0.0f), false, false);
		}
		else
		{
			LaunchCharacter(FVector(_pushbackAmount*2.0f, 0.0f, 0.0f), false, false);
		}
	}
	else
	{
		if (_launchAmount > 0.0f) {
			GetCharacterMovement()->GravityScale *= 0.7;
			characterState = ECharacterState::VE_Launched;
		}
		if (isFlipped)
		{
			LaunchCharacter(FVector(-_pushbackAmount, 0.0f, _launchAmount), false, false);
		}
		else
		{
			LaunchCharacter(FVector(_pushbackAmount, 0.0f, _launchAmount), false, false);
		}
	}
}
