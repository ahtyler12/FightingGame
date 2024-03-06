// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enums.h"
#include "Structs.h"
#include "CommandListEntry.h"
#include "Public/MainGameInstance.h"
#include "Containers/CircularBuffer.h"
#include "GameFramework/Character.h"
#include "Public/MainGameInstance.h"
#include "FightingGameCharacter.generated.h"

USTRUCT(BlueprintType)
struct FDefaultHurtboxDetails
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HurtBoxes")
		FVector scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HurtBoxes")
		FVector offset;
};




USTRUCT(BlueprintType)
struct FChargeInput
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EInputType inputType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int64 ChargeFrames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool IsHoldingInput;
};

USTRUCT(BlueprintType)
struct FInputInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		EInputType inputType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		EInputState inputState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		int64 inputFrame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		int64 chargeFrame = 0;
};

const int maxInputsPerFrame = 8;

struct FInputInfoArray
{
public:
	FInputInfo inputs[maxInputsPerFrame];
};


UCLASS(config=Game)
class AFightingGameCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AFightingGameCharacter();

	UFUNCTION(BlueprintImplementableEvent)
		void PlayDamageSoundEffect();
	/** Called for side to side input */
	void MoveRight(float Value);
	void MoveRightPad(float Value);
	UFUNCTION(BlueprintCallable)
		void addInputToInputbuffer(FInputInfo _inputInfo);
	UFUNCTION(BlueprintCallable)
		void CheckInputBufferForCommand(EInputState _inputState);
	UFUNCTION(BlueprintCallable)
	bool IsInputMultiInputCommand(FCommand _command, EInputType _pressedInput);
	UFUNCTION(BlueprintImplementableEvent)
	void PerformInputLogic(EInputType _type, EInputState _state);
	
	UFUNCTION(BlueprintCallable)
	void StartTrackingChargeTime(FChargeInput _trackedInput);

	UFUNCTION(BlueprintCallable)
	void ResetChargeTime(FChargeInput _trackedInput);

	UFUNCTION(BlueprintCallable)
		void DetermineCommandToUse();
	UFUNCTION(BlueprintCallable)
		void StartCommand(FString _commandName);
	UFUNCTION(BlueprintCallable)
		void AddtoInputMap(FString _input, EInputType _type);

	void StartLightAttack();
	void StartMediumAttack();
	void StartHeavyAttack();
	void StartSpecialAttack();
	void StartThrow();

	void PressMacro1();

	void ReleaseLightAttack();
	void ReleaseMediumAttack();
	void ReleaseHeavyAttack();
	void ReleaseSpecialAttack();

	void ReleaseMacro1();

	virtual void Tick(float DeltaTime) override;
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Landed(const FHitResult& Hit)override;

	void BeginStun();
	void EndStun();
	void BeginHitStop(int _hitStopFrames);
	void EndHitStop();

	UFUNCTION(BlueprintCallable)
		void StartCrouching();
	UFUNCTION(BlueprintCallable)
		void StopCrouching();
	UFUNCTION(BlueprintCallable)
		void StartBlocking();
	UFUNCTION(BlueprintCallable)
		void StopBlocking();
	UFUNCTION(BlueprintCallable)
		void CollidedWithProximityBox(int _blockstunFrames);
	//Move a character off the others hurtbox smoothly
	UFUNCTION(BlueprintImplementableEvent)
		void MoveCharacterSmoothly(FVector _start, FVector _end);
	UFUNCTION(BlueprintCallable)
	void CheckStuckInOtherCharacter();
	UFUNCTION(BlueprintCallable)
		void BeginCombo();
	UFUNCTION(BlueprintCallable)
		void EndCombo();
	UFUNCTION(BlueprintCallable)
		void DetermineComboRating();
	UFUNCTION(BlueprintCallable)
		void PerformPushBack(float _pushbackAmount, float _launchAmount, bool _isBlocking);
	/*Called when you've won a round*/
	UFUNCTION(BlueprintCallable)
		void WinRound();
	UFUNCTION(BlueprintCallable)
		void WinMatch();
	UFUNCTION(BlueprintImplementableEvent)
		void NotifyRoundEnd();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateHudRoundIcons();
	UFUNCTION(BlueprintCallable)
		void CustomLaunchPlayer(FVector _LaunchVelocity, bool _shouldOverrideXY, bool _shouldOverrideZ, bool _ShouldIgnorePlayer = false);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void IgnoreCollision(bool _shouldIgnore);

	/*Take Damage Function*/
	UFUNCTION(BlueprintCallable)
		void FGTakeDamage(float Damage, int _stunFrames, int _blockStunFrames, int _hitStopFrames,
			float _pushbackAmount, float _launchAmount, bool _shouldIgnoreStunDecay = false, bool _shouldIgnoreDamageDecay = false);
	UFUNCTION(BlueprintCallable)
		void SetHealth(float _health);
	UFUNCTION(BlueprintCallable)
		float GetMaxHealth();
	UFUNCTION(BlueprintCallable)
		void SetSuperMeter(float _meter);
	UFUNCTION(BlueprintCallable)
		float GetMaxSuperMeter();
	/*
		###############################################################################################################################
		###############################################################################################################################
														VARIABLES
		###############################################################################################################################
		###############################################################################################################################
	*/

	

	/*Other player reference*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
		AFightingGameCharacter* otherPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
		ECharacterClass characterClass;

	/*Is Mesh Flipped*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		bool isFlipped;
	/*Mesh Scale*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		FVector scale;
	/*Mesh Transform*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		FTransform transform;

	/* Checks direction Player is Facing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		bool isFacingRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		bool canFlip;
	
	/*Checks if player is stuck in enemy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		bool isStuckInEnemy;

	/*Hurtbox reference*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
		AActor* Hurtbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
		TArray<FDefaultHurtboxDetails> HurtboxDetails;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		TArray<UStaticMeshComponent*> SimpleHurtboxArray;

	/*Player Health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float playerHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float maxPlayerHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool takenDamageThisFrame = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		int lastStunFramesReceived;
	/*Keeps track of last damage received for practice mode*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float lastDamageReceived;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ECharacterState characterState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EComboState comboState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EAttackState attackState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EInvulType invulType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool isCrouching;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int stunFrames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int hitstopFrames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool shouldGroundBounce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool shouldWallBounce;
	//Number of frames where an Instant Block is considered valid
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		const int instantBlockWindow = 8;
	//Number of frames needed to pass to not get locked out of instant block
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		const int framesBetweenInstantBlockAttempt = 10;
		//Number of frames before another instant block can be performed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		int numFramesUntilInstantBlockAttempt;
		//The frame to start checking for an Instant Block. -1 indicates no opportunity for instant block
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		int startInstantBlockFrame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MaxDistanceApart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool canMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float gravityScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float gravityscaleModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float jumpHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float jumpDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		int jumpCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		int maxJumpCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool hasReleasedRightAxisInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool hasReleasedLeftAxisInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float forwardDashDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float backDashDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool isTouchingWall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool shouldHardKnockDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool wasExAttackUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool wasSuperAttackUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float superMeterAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float maxSuperMeterAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool hasLandedHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool hasLandedThrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool canAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FCommand lastCommandUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	int gattlingLevel = 0;

	//Combo Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
		float hitStunDecayModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
		float damageDecayModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
		int comboCounter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
		EComboRating lastComboRating;
	/*Could possibly combine in the even of not allowing combos to both ground and wall bounce*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
		bool hasUsedbounce;




		TCircularBuffer<FInputInfoArray> inputBuffer = TCircularBuffer<FInputInfoArray>(60);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
		float inputDecayTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
		TArray<FCommand> commandList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
		TArray<FCommand> moveBuffer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
		bool isPlayerOne;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
		TArray<FChargeInput> ChargeTimes;
	

	/* These variables are for the inputbuffer no need to have them visible to BPs
	*/
	int currentTick;
	int currentFrameInputIndex;
	bool capturedInputThisFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool playingOutro;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool playingSuper;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		bool roundLost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		int roundsWon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		bool canTakeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		bool wonMatch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		bool isPressingBack;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
};

