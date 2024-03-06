// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EInputType : uint8
{
	E_None				UMETA(DisplayName = "NONE"),
	E_Forward			UMETA(DisplayName = "FORWARD"),
	E_Back				UMETA(DisplayName = "BACKWARD"),
	E_Up				UMETA(DisplayName = "UP"),
	E_UpForward			UMETA(DisplayName = "UPFORWARD"),
	E_UpBack			UMETA(DisplayName = "UPBACK"),
	E_Down				UMETA(DisplayName = "DOWN"),
	E_DownForward		UMETA(DisplayName = "DOWNFORWARD"),
	E_DownBack			UMETA(DisplayName = "DOWNBACK"),
	E_Block				UMETA(DisplayName = "BLOCK"),
	E_Throw				UMETA(DisplayName = "THROW"),
	E_LightAttack		UMETA(DisplayName = "LIGHT ATTACK"),
	E_MediumAttack		UMETA(DisplayName = "MEDIUM ATTACK"),
	E_HeavyAttack		UMETA(DisplayName = "HEAVY ATTACK"),
	E_SpecialAttack		UMETA(DisplayName = "SPECIAL ATTACK")
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	E_Default		UMETA(DisplayName = "IDLE"),
	E_MovingForward	UMETA(DisplayName = "MOVING FORWARD"),
	E_DashForward	UMETA(DisplayName = "DASH FORWARD"),
	E_RunForward	UMETA(DisplayName = "RUN FORWARD"),
	E_MovingBack	UMETA(DisplayName = "MOVING BACKWARD"),
	E_DashBack		UMETA(DisplayName = "DASH BACK"),
	E_NeutralJump	UMETA(DisplayName = "NEUTRAL JUMP"),
	E_ForwardJump	UMETA(DisplayName = "FORWARD JUMP"),
	E_BackwardJump	UMETA(DisplayName = "BACKWARD JUMP"),
	E_WallJump		UMETA(DisplayName = "Wall Jump"),
	E_Blocking		UMETA(DisplayName = "BLOCKING"),
	E_Crouching		UMETA(DisplayName = "CROUCHING"),
	E_CrouchBlocking UMETA(DisplayName = "CROUCH BLOCKING")
};


UENUM(BlueprintType)
enum class EComboState : uint8
{
	E_None			UMETA(DisplayName = "None"),
	E_StandingStun	UMETA(DisplayName = "STAND STUN"),
	E_CrouchStun	UMETA(DisplayName = "CROUCH STUN"),
	E_AirStun		UMETA(DisplayName = "Air Stun"),
	E_Launched		UMETA(DisplayName = "LAUNCHED"),
	E_KnockDown		UMETA(DisplayName = "KNOCKEDDOWN"),
	E_WallBounce	UMETA(DisplayName = "WallBounce"),
	E_GroundBounce	UMETA(DisplayName = "GroundBounce"),
	E_Recovery		UMETA(DisplayName = "Recovery"),
	E_INSUPER		UMETA(DisplayName = "In Super")
};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	E_NONE				UMETA(DisplayName = "None"),
	E_ATTACKSTARTED		UMETA(DisplayName = "Attack Startup"),
	E_ATTACKACTIVE		UMETA(DisplayName = "Attack Active"),
	E_ATTACKRECOVERY	UMETA(DisplayName = "Attack Recovery"),
	E_GRABSTARTED		UMETA(DisplayName = "Grab Start"),
	E_GRABBREAK			UMETA(DisplayName = "Grab Break")
};

UENUM(BlueprintType)
enum class EInvulType : uint8
{
	E_None					UMETA(DisplayName = "No Invul"),
	E_ProjectileInvul		UMETA(DisplayName = "Projectile Invul"),
	E_StrikeInvul			UMETA(DisplayName = "Strike Invul"),
	E_Throw					UMETA(DisplayName = "Throw Invul"),
	E_FullInvul 			UMETA(DisplayName = "FullIvul")
};

UENUM(BlueprintType)
enum class EInputState : uint8
{
	E_None		 UMETA(DisplayName = "None"),
	E_Pressed	 UMETA(DisplayName = "Pressed"),
	E_Held		 UMETA(DisplayName = "Held"),
	E_Released	 UMETA(DisplayName = "Released")
};

/*Used to Determing Functionality during combos.
  If Attack is of the Combo Ender type then Combos are not allowed
*/
UENUM(BlueprintType)
enum class EComboType : uint8
{
	E_ComboStarter,
	E_ComboEnder
};

/**
 * 
 */
class FIGHTINGGAME_API Enums
{
public:

};
