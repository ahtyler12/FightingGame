// Copyright Epic Games, Inc. All Rights Reserved.

#include "FightingGameGameMode.h"
#include "FightingGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFightingGameGameMode::AFightingGameGameMode()
{
	roundTime = 1.0f;
	numRounds = 3;
	timerActive = false;
	matchState = EMatchState::E_Default;
	shouldSkipAllIntros = false;
}

void AFightingGameGameMode::CheatFullHealAll()
{
	if (player1)
	{
		player1->SetHealth(player1->GetMaxHealth());
	}
	if (player2)
	{
		player2->SetHealth(player2->GetMaxHealth());
	}
}

void AFightingGameGameMode::CheatFullHealthPlayer(int _playerNumber)
{
	if (_playerNumber == 1)
	{
		player1->SetHealth(player1->GetMaxHealth());
	}
	else if (_playerNumber == 2)
	{
		player2->SetHealth(player2->GetMaxHealth());
	}
}

void AFightingGameGameMode::CheatFullSuperAll()
{
	if (player1)
	{
		player1->SetSuperMeter(player1->GetMaxSuperMeter());
	}
	if (player2)
	{
		player2->SetSuperMeter(player2->GetMaxSuperMeter());
	}
}

void AFightingGameGameMode::Tick(float DeltaTime)
{
	BPTick(DeltaTime);

	if (hitStopFrames > 0)
	{
		--hitStopFrames;
		UE_LOG(LogTemp, Warning, TEXT("Stopped for %d more frames!"), hitStopFrames);
		if (hitStopFrames <= 0)
		{
			if (player1)
			{
				player1->EndHitStop();
			}
			if (player2)
			{
				player2->EndHitStop();
			}
		}
	}
}

void AFightingGameGameMode::CheatFullSuperPlayer(int _playerNumber)
{
	if (_playerNumber == 1)
	{
		player1->SetSuperMeter(player1->GetMaxSuperMeter());
	}
	else if (_playerNumber == 2)
	{
		player2->SetSuperMeter(player2->GetMaxSuperMeter());
	}
}


void AFightingGameGameMode::ResetCharacterHealth(AFightingGameCharacter* _charRef, bool _shouldResetAll)
{
	if (_shouldResetAll)
	{
		if (player1 && player2) 
		{
			player1->SetHealth(player1->GetMaxHealth());
			player2->SetHealth(player2->GetMaxHealth());
		}
	}
	else
	{
		if (_charRef != nullptr)
		{
			_charRef->SetHealth(_charRef->GetMaxHealth());
		}
	}
}

void AFightingGameGameMode::MatchWon(AFightingGameCharacter* _winningChar)
{
	matchState = EMatchState::E_MatchOver;
	if (auto gameInstance = Cast<UMainGameInstance>(GetGameInstance()))
	{
		gameInstance->SaveGame();
	}
}

void AFightingGameGameMode::RoundWon(AFightingGameCharacter* _winningChar)
{
	if (_winningChar)
	{
		_winningChar->WinRound();

		if (_winningChar->roundsWon == numRounds)
		{
			MatchWon(_winningChar);
		}
		_winningChar->NotifyRoundEnd();
		_winningChar->UpdateHudRoundIcons();
	}
}

AFightingGameCharacter* AFightingGameGameMode::getOtherPlayer(AFightingGameCharacter* requester)
{
	if (requester == player1)
	{	UE_LOG(LogTemp, Warning, TEXT("Returning Player 2"));
		return player2;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Returning Player 1"));
		return player1;
	}
}
