// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FightingGameCharacter.h"
#include "FightingGameGameMode.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8 {
	E_Default			UMETA(DisplayName = "No State"),
	E_StageIntro		UMETA(DisplayName = "Stage Intro"),
	E_CharacterIntroA	UMETA(DisplayName = "Character Intro A"),
	E_CharacterIntroB	UMETA(DisplayName = "Character Intro B"),
	E_MatchBegin		UMETA(DisplayName = "Match Begin"),
	E_MatchActive		UMETA(DisplayName = "Match Active"),
	E_RoundOver			UMETA(DisplayName = "Round Over"),
	E_MatchOver			UMETA(DisplayName = "Match Over"),
	E_CharacterDefeat	UMETA(DisplayName = "Character Defeat"),
	E_CharacterVictory	UMETA(DisplayName = "Character Victory"),
	E_MatchResults		UMETA(DisplayName = "Match Results")
};

UCLASS(minimalapi)
class AFightingGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFightingGameGameMode();
	UFUNCTION(Exec, Category = ExecFunctions)
		void CheatFullHealAll();
	UFUNCTION(Exec, Category = ExecFunctions)
		void CheatFullHealthPlayer(int _playerNumber);
	UFUNCTION(Exec, Category = ExecFunctions)
		void CheatFullSuperAll();
	UFUNCTION(Exec, Category = ExecFunctions)
		void CheatFullSuperPlayer(int _playerNumber);
	UFUNCTION(BlueprintImplementableEvent)
		void skipStageIntro();
	UFUNCTION(BlueprintImplementableEvent)
		void skipPlayerIntro();
	UFUNCTION(BlueprintImplementableEvent)
		void PauseGame(AFightingGameController* _owningPlayer);
	UFUNCTION(BlueprintImplementableEvent)
		void DisplaySessionInfo(const FString& _sessionId);
	UFUNCTION(BlueprintCallable)
		void ResetCharacterHealth(AFightingGameCharacter* _charRef = nullptr, bool _shouldResetAll = true);
	UFUNCTION(BlueprintCallable)
		void MatchWon(AFightingGameCharacter* _winningChar = nullptr);
	UFUNCTION(BlueprintCallable)
		void RoundWon(AFightingGameCharacter* _winningChar = nullptr);
	UFUNCTION(BlueprintCallable)
		AFightingGameCharacter* getOtherPlayer(AFightingGameCharacter* requester);

	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintImplementableEvent)
	void BPTick(float DeltaTime);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player References")
		AFightingGameCharacter* player1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player References")
		AFightingGameCharacter* player2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		bool timerActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		bool shouldSkipAllIntros;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		int numRounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		float roundTime;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
		int hitStopFrames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match State")
		EMatchState matchState;
		
};



