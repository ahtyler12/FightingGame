// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Components/AudioComponent.h"
#include "Engine/DataTable.h"
#include "Enums.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */



UENUM(BlueprintType)
enum class EGameMode : uint8
{
	E_Arcade UMETA(DisplayName = "Arcade Mode"),
	E_Versus UMETA(DisplayName = "Versus Mode"),
	E_Options UMETA(DisplayName = "Options"),
	E_ExitGame UMETA(DisplayName = "Close Game"),
};

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	E_Yusuke  UMETA(DisplayName = "Yusuke"),
	E_Kurama  UMETA(DisplayName = "Kurama"),
	E_Hiei	  UMETA(DisplayName = "Hiei"),
	E_Kuwabara UMETA(DisplayName = "Kuwabara"),
	E_Suzaku  UMETA(DisplayName = "Suzaku"),
	E_Toguro  UMETA(DisplayName = "Toguro"),
	E_Sensui  UMETA(DisplayName = "Sensui"),
	E_Mukuro  UMETA(DisplayName = "Mukuro")
};

UENUM(BlueprintType)
enum class EComboRating : uint8
{
	E_Default UMETA(DisplayName = "NONE"),
	E_Good	  UMETA(DisplayName = "GOOD"),
	E_Great	  UMETA(DisplayName = "GREAT"),
	E_Execel  UMETA(DisplayName = "EXECELENT"),
	E_Awesome UMETA(DisplayName = "AWESOME"),
	E_Ultimate UMETA(DisplayName = "ULTIMATE")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FControllerConnectionChanged, bool, isConnected);

UCLASS()
class FIGHTINGGAME_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMainGameInstance();
	virtual void Init() override;
	/*Start Save Block*/
	void CreateSaveFile();
	void SaveGame();
	void LoadGame();
	/*End Save Block*/


	void ControllerConnectionChanged(bool _isConnected, FPlatformUserId _plarformID, int32 _userID);

	void GenericControllerConnectionChanged(EInputDeviceConnectionState _connectionState, FPlatformUserId _userID, FInputDeviceId _deviceID);
	
	/*Start Online Block*/
	UFUNCTION(BlueprintCallable)
		void HostSession(FName sessionName = "My Session");
	UFUNCTION(BlueprintCallable)
	void SearchForSessions();

	void SearchForSessionsCompleted (bool _searchComplete);
	void HostSessionCompleted(FName sessionName,bool succeeded);
	void JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/*Online Mode Variables*/	
	IOnlineSessionPtr sessionInterface;
	TSharedPtr<FOnlineSessionSearch> searchSettings;
	TArray<FOnlineSessionSearchResult> searchResults;
	FOnFindSessionsCompleteDelegate SearchForSessionsCompletedDelegate;
	FOnCreateSessionCompleteDelegate HostSessionCompletedDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompletedDelegate;
	FDelegateHandle SearchForSessionsCompletedHandle;

	
	/*End Online Block*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	TArray<EInputType> P1Macros;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inputs")
	TArray<EInputType> P2Macros;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerReferences")
	ECharacterClass p1CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerReferences")
	ECharacterClass p2CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
	bool isDeviceMultiInput;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Controller")
	FControllerConnectionChanged ControllerChangedEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
	int numRounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Flow")
	float roundTime;
};
