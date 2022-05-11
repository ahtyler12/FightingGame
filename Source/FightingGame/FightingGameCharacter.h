// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FightingGameCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	VE_Default		UMETA(DisplayName = "NOT_MOVING"),
	VE_MovingRight	UMETA(DisplayName = "MOVING_RIGHT"),
	VE_MovingLeft	UMETA(DisplayName = "MOVING_LEFT"),
	VE_Jumping      UMETA(DisplayName = "JUMPING"),
	VE_Crouching    UMETA(DisplayName = "CROUCHING"),
	VE_Stunned		UMETA(DisplayName = "STUNNED"),
	VE_Blocking		UMETA(DisplayName = "BLOCKING"),
	VE_Launched		UMETA(DisplayName = "LAUNCHED")
};

USTRUCT(BlueprintType)
struct FInputInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FString inputName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		float inputTime;
};





UCLASS(config=Game)
class AFightingGameCharacter : public ACharacter
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable)
		void addInputToInputbuffer(FInputInfo _inputInfo);
	UFUNCTION(BlueprintCallable)
		void removeInputFromBuffer();

	void StartLightAttack();
	void StartMediumAttack();
	void StartHeavyAttack();
	void StartSpecialAttack();
	void StartExAttack();

	UFUNCTION(BlueprintCallable)
		void P2StartLightAttack();
	UFUNCTION(BlueprintCallable)
		void P2StartMediumAttack();
	UFUNCTION(BlueprintCallable)
		void P2StartHeavyAttack();
	UFUNCTION(BlueprintCallable)
		void P2StartSpecialAttack();
	UFUNCTION(BlueprintCallable)
		void P2Jump();
	UFUNCTION(BlueprintCallable)
		void P2StopJump();
	UFUNCTION(BlueprintCallable)
		void P2StartExAttack();
	UFUNCTION(BlueprintCallable)
		void P2MoveRight(float _value);

	virtual void Tick(float DeltaTime) override;
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Landed(const FHitResult& Hit)override;

	void BeginStun();
	void EndStun();

	UFUNCTION(BlueprintCallable)
		void StartCrouching();
	UFUNCTION(BlueprintCallable)
		void StopCrouching();
	UFUNCTION(BlueprintCallable)
		void StartBlocking();
	UFUNCTION(BlueprintCallable)
		void StopBlocking();
	UFUNCTION(BlueprintCallable)
		void CollidedWithProximityBox();
	UFUNCTION(BlueprintCallable)
		void PerformPushBack(float _pushbackAmount, float _launchAmount, bool _isBlocking);


public:
	AFightingGameCharacter();


protected:

	/** Called for side to side input */
	void MoveRight(float Value);

	/*Other player reference*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
		AFightingGameCharacter* otherPlayer;

	/*Is Mesh Flipped*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		bool isFlipped;
	/*Mesh Scale*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		FVector scale;
	/*Mesh Transform*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		FTransform transform;

	/*Hurtbox reference*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
		AActor* Hurtbox;

	/*Take Damage Function*/
	UFUNCTION(BlueprintCallable)
	void FGTakeDamage(float Damage, float _stunTime, float _blockStunTime, float _pushbackAmount, float _launchAmount);
	
	/*Player Health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float playerHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		ECharacterState characterState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MaxDistanceApart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool isCrouching;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float stunTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FTimerHandle stunHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool canMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float gravityScale;
	/**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool wasLightAttackUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool wasMediumAttackUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool wasHeavyAttackUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool wasSpecialAttackUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool wasExAttackUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool wasSuperAttackUsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float superMeterAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool hasLandedHit;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
		TArray<FInputInfo> inputBuffer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
		FTimerHandle inputBufferHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
		float inputDecayTime;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
};

