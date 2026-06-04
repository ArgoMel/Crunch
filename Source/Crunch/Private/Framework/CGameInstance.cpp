// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CGameInstance.h"
#include "Network/CNetStatics.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "HttpModule.h"
#include "Crunch/Crunch.h"

void UCGameInstance::Init()
{
	Super::Init();
	if (GetWorld()->IsEditorWorld())
	{
		return;
	}
	if (UCNetStatics::IsSessionServer(this))
	{
		CreateSession();
	}
}

bool UCGameInstance::IsLoggedIn() const
{
	if (const IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr(GetWorld()))
	{
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}

	return false;
}

bool UCGameInstance::IsLoggingIn() const
{
	return LoggingInDelegateHandle.IsValid();
}

void UCGameInstance::ClientAccountPortalLogin()
{
	ClientLogin("AccountPortal", "", "");
}

void UCGameInstance::ClientLogin(const FString& Type, const FString& Id, const FString& Token)
{
	if (const IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr(this))
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		LoggingInDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &ThisClass::LoginCompleted);
		if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, Id, Token)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Login Failed Right Away!"))
			if (LoggingInDelegateHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
				LoggingInDelegateHandle.Reset();
			}
			OnLoginCompleted.Broadcast(false, "", "Login Failed Right Away!");
		}
	}
}

void UCGameInstance::LoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (const IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr(this))
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		FString PlayerNickname = "";
		if (bWasSuccessful)
		{
			PlayerNickname = IdentityPtr->GetPlayerNickname(UserId);
			UE_LOG(LogTemp, Warning, TEXT("Logged in successfully as: %s"), *(PlayerNickname))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Logging in failed: %s"), *(Error))
		}

		OnLoginCompleted.Broadcast(bWasSuccessful, PlayerNickname, Error);
	}
	else
	{
		OnLoginCompleted.Broadcast(false, "", "Can't find the Identity Pointer");
	}
}

void UCGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Requesting Create and Join Session: %s"), *(NewSessionName.ToString()))
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchId = FGuid::NewGuid();

	const FString CoordinatorURL = UCNetStatics::GetCoordinatorURL();

	const FString URL = FString::Printf(TEXT("%s/Sessions"), *CoordinatorURL);
	UE_LOG(LogTemp, Warning, TEXT("Sending Request Session Creation to URL: %s"), *URL)

	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(Crunch::Session::SessionName.ToString(), NewSessionName.ToString());
	JsonObject->SetStringField(Crunch::Session::SessionSearchID.ToString(), SessionSearchId.ToString());

	FString requestLobby;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&requestLobby);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(requestLobby);
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SessionCreationRequestCompleted, SessionSearchId);
	
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Request Failed Right Away!"))
	}
}

void UCGameInstance::CancelSessionCreation()
{
	UE_LOG(LogTemp, Warning, TEXT("Canceling Session Creation"))
	StopAllSessionFindings();

	if (const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}

	StartGlobalSessionSearch();
}

void UCGameInstance::StartGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Global Session Search"))
	GetWorld()->GetTimerManager().SetTimer(GlobalSessionSearchTimerHandle, this, &ThisClass::FindGlobalSessions, GlobalSessionSearchInterval, true, 0.f);
}

bool UCGameInstance::JoinSessionWithId(const FString& SessionIdStr)
{
	if (SessionSearch.IsValid())
	{
		const FOnlineSessionSearchResult* SessionSearchResult = SessionSearch->SearchResults.FindByPredicate(
			[=](const FOnlineSessionSearchResult& Result)
			{
				return Result.GetSessionIdStr() == SessionIdStr;
			}
		);
		if (SessionSearchResult)
		{
			JoinSessionWithSearchResult(*SessionSearchResult);
			return true;
		}
	}

	return false;
}

void UCGameInstance::SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully, FGuid SessionSearchId)
{
	if (!bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection responded with connection was not successful!"))
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Connection to Coordinator Successfully!"))

	const int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Failed, with code: %d"), ResponseCode)
		return;
	}

	const FString ResponseStr = Response->GetContentAsString();
	
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
	int32 Port = 0;

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		Port = JsonObject->GetIntegerField(*(Crunch::Session::PortKey.ToString()));
	}

	UE_LOG(LogTemp, Warning, TEXT("Connected to Coordinator Successfully and the new session created is on port: %d"), Port)
	StartFindingCreatedSession(SessionSearchId);
}

void UCGameInstance::StartFindingCreatedSession(const FGuid& SessionSearchId)
{
	if (!SessionSearchId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Search Id is invalid, can't start finding!"))
		return;
	}

	StopAllSessionFindings();
	UE_LOG(LogTemp, Warning, TEXT("Start Finding Created Session with Id: %s"), *(SessionSearchId.ToString()))

	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimerHandle, 
		FTimerDelegate::CreateUObject(this, &ThisClass::FindCreatedSession, SessionSearchId),
		FindCreatedSessionSearchInterval,
		true, 0.f
		);

	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimeoutTimerHandle, this, &ThisClass::FindCreatedSessionTimeout, FindCreatedSessionTimeoutDuration);
}

void UCGameInstance::StopAllSessionFindings()
{
	UE_LOG(LogTemp, Warning, TEXT("Stoping All Session Search"))
	StopFindingCreatedSession();
	StopGlobalSessionSearch();
}

void UCGameInstance::StopFindingCreatedSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Finding Created Session"))
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimeoutTimerHandle);

	if (const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::StopGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Global Session Search"))
	if (GlobalSessionSearchTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GlobalSessionSearchTimerHandle);
	}

	const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this);
	if (SessionPtr)
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::FindGlobalSessions()
{
	UE_LOG(LogTemp, Warning, TEXT("----- Retrying Global Session Search -------------"))

	const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this);
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Session Interface, Wait for the next Global Session Search"))
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;
	
	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::GlobalSessionSearchCompleted);
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Global Session Failed Right Away"))
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::GlobalSessionSearchCompleted(bool bWasSuccessful) const
{
	if (bWasSuccessful)
	{
		OnGlobalSessionSearchCompleted.Broadcast(SessionSearch->SearchResults);
		for (const FOnlineSessionSearchResult& OnlineSessionSearchResult : SessionSearch->SearchResults)
		{
			FString SessionName = "Name_None";
			OnlineSessionSearchResult.Session.SessionSettings.Get<FString>(Crunch::Session::SessionName, SessionName);
			UE_LOG(LogTemp, Warning, TEXT("Found Session: %s after global session search"), *SessionName)
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Global Session Search Completed Unsuccessfully"))
	}

	const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this);
	if (SessionPtr)
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::FindCreatedSession(FGuid SessionSearchId)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to find created session"))
	const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this);
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find Session Ptr, canceling session search"))
		return;
	}
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	if (!SessionSearch)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to create session search!, canceling session search"))
		return;
	}

	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 1;
	SessionSearch->QuerySettings.Set(Crunch::Session::SessionSearchID, SessionSearchId.ToString(), EOnlineComparisonOp::Equals);

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::FindCreateSessionCompleted);
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Session Failed Right Away!..."))
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::FindCreatedSessionTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("Find Created Session Timeout Reached"))
	StopFindingCreatedSession();
}

void UCGameInstance::FindCreateSessionCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionSearch->SearchResults.IsEmpty())
	{
		return;
	}

	StopFindingCreatedSession();
	JoinSessionWithSearchResult(SessionSearch->SearchResults[0]);
}

void UCGameInstance::JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Joining session with Search Result"))
	const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this);
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Session Ptr, Cancel Joining"))
		return;
	}
	
	FString SessionName = "";
	SearchResult.Session.SessionSettings.Get<FString>(Crunch::Session::SessionName, SessionName);

	const FOnlineSessionSetting* PortSetting = SearchResult.Session.SessionSettings.Settings.Find(Crunch::Session::PortKey);
	int64 Port = 7777;
	PortSetting->Data.GetValue(Port);

	UE_LOG(LogTemp, Warning, TEXT("Trying to join session: %s, at port: %lld"), *(SessionName), Port)
	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::JoinSessionCompleted, static_cast<int>(Port));
	if (!SessionPtr->JoinSession(0, FName(SessionName), SearchResult))
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining Session Failed Right Away! ....."))
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		OnJoinSessionFailed.Broadcast();
	}
}

void UCGameInstance::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port)
{
	const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this);
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session Completed, but can't find session pointer"))
		OnJoinSessionFailed.Broadcast();
		return;
	}

	if (JoinResult == EOnJoinSessionCompleteResult::Success)
	{
		StopAllSessionFindings();
		UE_LOG(LogTemp, Warning, TEXT("Joining Session: %s successful, the port is: %d"), *(SessionName.ToString()), Port)

		FString TravelURL = "";
		SessionPtr->GetResolvedConnectString(SessionName, TravelURL);
		
		FString TestingURL = UCNetStatics::GetTestingURL();
		if (!TestingURL.IsEmpty())
		{
			TravelURL = TestingURL;
		}

		UCNetStatics::ReplacePort(TravelURL, Port);

		UE_LOG(LogTemp, Warning, TEXT("Traveling to Session at: %s"), *TravelURL)

		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, TRAVEL_Absolute);
	}
	else
	{
		OnJoinSessionFailed.Broadcast();
	}

	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
}

void UCGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	if (WaitPlayerJoinTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	}

	PlayerRecord.Add(UniqueId);
}

void UCGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("All player left the session, terminating"))
		TerminateSessionServer();
	}
}

void UCGameInstance::CreateSession()
{
	const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this);
	if (SessionPtr)
	{
		ServerSessionName = UCNetStatics::GetSessionNameStr();
		const FString SessionSearchId = UCNetStatics::GetSessionSearchIdStr();
		SessionServerPort = UCNetStatics::GetSessionPort();
		UE_LOG(LogTemp, Warning, TEXT("#### Create Session With Name: %s, ID: %s, Port: %d"), *(ServerSessionName), *(SessionSearchId), SessionServerPort)

		const FOnlineSessionSettings OnlineSessionSetting = UCNetStatics::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionSearchId, SessionServerPort);
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnSessionCreated);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSetting))
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Creating Failed Right away!!!!"))
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find session ptr, terminating"))
		TerminateSessionServer();
	}
}

void UCGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("------------- Session Created!"))
		GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinTimeoutHandle, this, &ThisClass::WaitPlayerJoinTimeoutReached, WaitPlayerJoinTimeOutDuration);
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("------------ Session Creation Failed"))
		TerminateSessionServer();
	}

	if (const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::EndSessionCompleted(FName SessionName, bool bWasSuccessful) const
{
	FGenericPlatformMisc::RequestExit(false);
}

void UCGameInstance::TerminateSessionServer()
{
	if (const IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &ThisClass::EndSessionCompleted);
		if (!SessionPtr->EndSession(FName{ ServerSessionName }))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCGameInstance::WaitPlayerJoinTimeoutReached()
{
	UE_LOG(LogTemp, Warning, TEXT("Session Sever shut down after %f seconds without player joining"), WaitPlayerJoinTimeOutDuration)
	TerminateSessionServer();
}

void UCGameInstance::StartMatch() const
{
	if (GetWorld()->GetNetMode() == NM_DedicatedServer 
		|| GetWorld()->GetNetMode() == NM_ListenServer)
	{
		LoadLevelAndListen(GameLevel);
	}
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level) const
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));

	if (LevelURL != "")
	{
		const FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), SessionServerPort);
		UE_LOG(LogTemp, Warning, TEXT("Server Traveling to: %s"), *(TravelStr))
		GetWorld()->ServerTravel(TravelStr);
	}
}
