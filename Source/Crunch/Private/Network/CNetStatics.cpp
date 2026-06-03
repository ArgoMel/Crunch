// Fill out your copyright notice in the Description page of Project Settings.

#include "Network/CNetStatics.h"

#include "OnlineSubsystemUtils.h"
#include "Crunch/Crunch.h"

FOnlineSessionSettings UCNetStatics::GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int Port)
{
	FOnlineSessionSettings OnlineSessionSettings{};
	OnlineSessionSettings.bIsLANMatch = false;
	OnlineSessionSettings.NumPublicConnections = Crunch::ConstValue::MaxPlayerCount * 2;
	OnlineSessionSettings.bShouldAdvertise = true;
	OnlineSessionSettings.bUsesPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	OnlineSessionSettings.bAllowInvites = true;
	OnlineSessionSettings.bAllowJoinInProgress = false;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	OnlineSessionSettings.bUsesStats = true;

	OnlineSessionSettings.Set(Crunch::Session::SessionName, SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(Crunch::Session::SessionSearchID, SessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(Crunch::Session::PortKey, Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	return OnlineSessionSettings;
}

IOnlineSessionPtr UCNetStatics::GetSessionPtr(const UObject* WorldContextObject)
{
	const UWorld* world=WorldContextObject->GetWorld();
	check(world);
	const IOnlineSubsystem* OnlineSubSystem = Online::GetSubsystem(world);
	if (OnlineSubSystem)
	{
		return OnlineSubSystem->GetSessionInterface();
	}
	return nullptr;
}

IOnlineIdentityPtr UCNetStatics::GetIdentityPtr(const UObject* WorldContextObject)
{
	const UWorld* world=WorldContextObject->GetWorld();
	check(world);
	const IOnlineSubsystem* OnlineSubSystem = Online::GetSubsystem(world);
	if (OnlineSubSystem)
	{
		return OnlineSubSystem->GetIdentityInterface();
	}
	return nullptr;
}

bool UCNetStatics::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetNetMode() == NM_DedicatedServer;
}

FString UCNetStatics::GetSessionNameStr()
{
	return GetCommandlineArgAsString(Crunch::Session::SessionName);
}

FString UCNetStatics::GetSessionSearchIdStr()
{
	return GetCommandlineArgAsString(Crunch::Session::SessionSearchID);
}

int UCNetStatics::GetSessionPort()
{
	return GetCommandlineArgAsInt(Crunch::Session::PortKey);
}

FString UCNetStatics::GetCoordinatorURL()
{
	FString CoordinatorURL = GetCommandlineArgAsString(Crunch::Session::CoordinatorURLKey);
	if (!CoordinatorURL.IsEmpty())
	{
		return CoordinatorURL;
	}

	return GetDefaultCoordinatorURL();
}

FString UCNetStatics::GetDefaultCoordinatorURL()
{
	FString CoordinatorURL = "";
	GConfig->GetString(TEXT("Crunch.Net"), TEXT("CoordinatorURL"), CoordinatorURL, GGameIni);
	UE_LOG(LogTemp, Warning, TEXT("Getting Default Coordinator URL as: %s"), *CoordinatorURL)
	return CoordinatorURL;
}

FString UCNetStatics::GetCommandlineArgAsString(const FName& ParamName)
{
	FString OutVal = "";
	const FString CommandLineArg = FString::Printf(TEXT("%s="), *ParamName.ToString());
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UCNetStatics::GetCommandlineArgAsInt(const FName& ParamName)
{
	int OutVal = 0;
	const FString CommandLineArg = FString::Printf(TEXT("%s="), *ParamName.ToString());
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

FString UCNetStatics::GetTestingURL()
{
	FString TestURL = GetCommandlineArgAsString(Crunch::Session::TestingURLKey);
	UE_LOG(LogTemp, Warning, TEXT("Get Testing URL: %s"), *TestURL)
	return TestURL;
}

void UCNetStatics::ReplacePort(FString& OutURLStr, int NewPort)
{
	FURL URL(nullptr, *OutURLStr, TRAVEL_Absolute);
	URL.Port = NewPort;
	OutURLStr = URL.ToString();
}
