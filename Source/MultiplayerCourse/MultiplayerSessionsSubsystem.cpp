// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

void PrintString(const FString &Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, Message);
    }
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
    bCreateServerAfterDestroy = false;
    DestroyedServerName = "";
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    IOnlineSubsystem *OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        // Steam, Google, etc
        // Se não conseguir usar Steam, Unreal automaticamente tenrará usar NULL
        // Para usar Steam precisamos estar com ela aberta no PC e logado
        FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
        PrintString(SubsystemName);

        SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
        }
    }
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
}

void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName)
{
    if (ServerName.IsEmpty())
    {
        PrintString("ServerName cannot be empty.");
        return;
    }

    // Session name deve ser único
    FName SessionName = FName("Co-op Session Name");

    // Destruímos uma sessão se ela já existe
    FNamedOnlineSession *ExistingSession = SessionInterface->GetNamedSession(SessionName);
    if (ExistingSession)
    {
        PrintString(FString::Printf(TEXT("Session %s already exists, destroying it..."), *SessionName.ToString()));
        bCreateServerAfterDestroy = true;
        DestroyedServerName = ServerName;

        SessionInterface->DestroySession(SessionName);
        // Retornamos porque DestroySession é async
        return;
    }

    // Settings
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.NumPublicConnections = 2;

    // vv Funções da Steam vv
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinViaPresence = true;
    // ^^ Funções da Steam ^^

    // Cria sessão como LAN se não conseguir conectar com a Steam
    SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

    SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
    if (ServerName.IsEmpty())
    {
        PrintString("Serve name cannot be empty.");
        return;
    }

    // Settings
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    SessionSearch->MaxSearchResults = 9999;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
    }
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bCreateServerAfterDestroy)
    {
        PrintString("Session destroyed successufully. Creating a new one.");
        bCreateServerAfterDestroy = false;
        CreateServer(DestroyedServerName);
    }
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (!bWasSuccessful || !SessionSearch)
    {
        return;
    }

    TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;

    if (Results.Num() > 0)
    {
        FString Message = FString::Printf(TEXT("%d sessions found."), Results.Num());
        PrintString(Message);
    } else {
        PrintString("Zero sessions found.");
    }
}
