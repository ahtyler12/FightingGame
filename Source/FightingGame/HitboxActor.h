// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitboxActor.generated.h"


UENUM(BlueprintType)
enum class EHitboxEnum :uint8
{
	HB_Proximity UMETA(DisplayName = "Proximity"),
	HB_Strike    UMETA(DisplayName = "Strike"),
	HB_HurtBox   UMETA(DisplayName = "Hurtbox")
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

	/*The Hitbox Type*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		EHitboxEnum hitboxType;
	/* The Hitbox Location*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		FVector hitboxLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		float hitboxDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		float hitStunTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		float blockStunTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		float pushbackAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
		float launchAmount;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




};
