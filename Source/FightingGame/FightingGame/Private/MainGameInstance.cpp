// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "Misc/CoreDelegates.h"
#include "../FightingGameGameMode.h"
#include "SaveGameData.h"
#include "Kismet/GameplayStatics.h"


UMainGameInstance::UMainGameInstance()
{
	numRounds = 3;
	roundTime = -1;


	/*Delete These Later*/
	P1Macros.SetNum(2);
	P1Macros[0] = EInputType::E_LightAttack;
	P1Macros[1] = EInputType::E_MediumAttack;

	P2Macros.SetNum(2);
	P2Macros[0] = EInputType::E_LightAttack;
	P2Macros[1] = EInputType::E_MediumAttack;

	isDeviceMultiInput = true;

	SearchForSessionsCompletedDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMainGameInstance::SearchForSessionsCompleted);
	HostSessionCompletedDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMainGameInstance::HostSessionCompleted);
	JoinSessionCompletedDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMainGameInstance::JoinSessionCompleted);
}

void UMainGameInstance::Init()
{
	Super::Init();
	searchResults.Empty();
	IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().AddUObject(this, &UMainGameInstance::GenericControllerConnectionChanged);
	LoadGame();
}

void UMainGameInstance::CreateSaveFile()
{
	USaveGameData* dataToSave = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
	UGameplayStatics::SaveGameToSlot(dataToSave, "Slot1", 0);
}

void UMainGameInstance::SaveGame()
{
	USaveGameData* dataToSave = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot("Slot1",0));

	if (dataToSave != nullptr)
	{
		/*Add Save Game Data Here*/
		UGameplayStatics::SaveGameToSlot(dataToSave, "Slot1", 0);
	}
	else if (!UGameplayStatics::DoesSaveGameExist("Slot1", 0))
	{
		CreateSaveFile();
	}
}

void UMainGameInstance::LoadGame()
{
	USaveGameData* dataToLoad = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot("Slot1", 0));

	if (dataToLoad != nullptr)
	{
		/*Add Load Game Data Here*/
	}
	else if (!UGameplayStatics::DoesSaveGameExist("Slot1", 0))
	{
		CreateSaveFile();
	}
}

void UMainGameInstance::ControllerConnectionChanged(bool _isConnected, FPlatformUserId _plarformID, int32 _userID)
{
	ControllerChangedEvent.Broadcast(_isConnected);

	if (_isConnected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller has been connected for player %d"), _userID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller has been disconnected for player %d"), _userID);
	}
}

void UMainGameInstance::GenericControllerConnectionChanged(EInputDeviceConnectionState _connectionState, FPlatformUserId _userID, FInputDeviceId _deviceID)
{
	if (_connectionState == EInputDeviceConnectionState::Connected)
	{
		ControllerChangedEvent.Broadcast(true);
	}
	else if (_connectionState == EInputDeviceConnectionState::Disconnected)
	{
		ControllerChangedEvent.Broadcast(false);
	}

	if (_connectionState == EInputDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller has been connected for player %d"), _userID);
	}
	else if (_connectionState == EInputDeviceConnectionState::Disconnected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller has been disconnected for player %d"), _userID);
	}
}
void UMainGameInstance::HostSession(FName sessionName)
{
	if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			TSharedPtr<FOnlineSessionSettings> sessionSettings = MakeShareable(new FOnlineSessionSettings());
			sessionSettings->bAllowInvites = true;
			sessionSettings->bAllowJoinInProgress = true;
			sessionSettings->bAllowJoinViaPresence = true;
			sessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
			sessionSettings->bIsDedicated = false;
			sessionSettings->bUsesPresence = true;
			sessionSettings->bIsLANMatch = false;
			sessionSettings->bShouldAdvertise = true;
			sessionSettings->NumPrivateConnections = 0;
			sessionSettings->NumPublicConnections = 2;

			const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if (onlineSessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), sessionName, *sessionSettings))
			{
				GEngine->AddOnScreenDebugMessage(0, 30.0f, FColor::Cyan, FString::Printf(TEXT("A session has been created!")));
				UE_LOG(LogTemp, Warning, TEXT("Session Has Been Created!"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(0, 30.0f, FColor::Cyan, FString::Printf(TEXT("Failed To Create Session!")));
				UE_LOG(LogTemp, Warning, TEXT("Failed To Create Session!"));
			}
		}
	}
}

void UMainGameInstance::SearchForSessions()
{
	if (IOnlineSubsystem* onlineSubSystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubSystem->GetSessionInterface())\
		{
			SearchForSessionsCompletedHandle = onlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(SearchForSessionsCompletedDelegate);
			searchSettings = MakeShareable(new FOnlineSessionSearch());
			searchSettings->bIsLanQuery = false;
			searchSettings->MaxSearchResults = 5;
			searchSettings->PingBucketSize = 30;
			searchSettings->TimeoutInSeconds = 10.0f;

			const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if (onlineSessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), searchSettings.ToSharedRef()))
			{

			}
		}
	}

}

void UMainGameInstance::SearchForSessionsCompleted(bool _searchComplete)
{
	if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (sessionInterface = onlineSubsystem->GetSessionInterface())
		{
			searchResults = searchSettings->SearchResults;
			sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(SearchForSessionsCompletedHandle);
			GEngine->AddOnScreenDebugMessage(0, 30.0f, FColor::Cyan, FString::Printf(TEXT("Search Found %d Sessions!"), searchSettings->SearchResults.Num()));
		}
		if (auto gameMode = Cast<AFightingGameGameMode>(GetWorld()->GetAuthGameMode()))
		{
			for (int i = 0; i < searchSettings->SearchResults.Num(); ++i)
			{				
				gameMode->DisplaySessionInfo(searchSettings->SearchResults[i].Session.GetSessionIdStr());
			}
		}

	}
}

void UMainGameInstance::HostSessionCompleted(FName sessionName,bool succeeded)
{
	if (succeeded)
	{

	}
}

void UMainGameInstance::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	FString JoinAddress = "";

	sessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
	GEngine->AddOnScreenDebugMessage(0, 30.0f, FColor::Cyan, FString::Printf(TEXT("Join Session Completed!")));
}
