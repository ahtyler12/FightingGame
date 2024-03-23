// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "HitboxActor.generated.h"


UENUM(BlueprintType)
enum class EHitboxEnum :uint8
{
	HB_Proximity UMETA(DisplayName = "Proximity"),
	HB_Strike    UMETA(DisplayName = "Strike"),
	HB_Throw     UMETA(DisplayName = "Throw"),
	HB_HurtBox   UMETA(DisplayName = "Hurtbox")
};

UENUM(BlueprintType)
enum class EHitboxHeight : uint8
{
	E_Low	UMETA(DisplayName = "Low"),
	E_Mid	UMETA(DisplayName = "Mid"),
	E_High	UMETA(DisplayName = "High"),
	E_None	UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct FHitboxData : public FTableRowBase
{
	GENERATED_BODY()
public:

	/*The Hitbox Type*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		EHitboxEnum hitboxType;
	/* The Hitbox Location*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		FVector hitboxLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		FVector scale = FVector(1.0f, 0.1f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		float hitboxDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		int hitStunFrames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		int blockStunFrames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		int hitstopFrames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		float pushbackAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		float launchAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		bool shouldIgnoreDamageDecay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		bool shouldIgnoreStunDecay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		bool shouldAttach;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		bool shouldWallBounce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		bool shouldGroundBounce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
	EHitboxHeight hitboxHeight;
};


UCLASS()
class FIGHTINGGAME_API AHitboxActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	AHitboxActor();
	UFUNCTION(BlueprintCallable)
	void TriggerVisualizeHitBox();
	UFUNCTION(BlueprintImplementableEvent)
	void VisualizeHitbox();

	UPROPERTY(BlueprintReadWrite, Category = "Hitbox")
		FHitboxData hitboxData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




};
