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
    ServerNameToFind = "";
    MySessionName = FName("Co-op Session Name");
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
        PrintString(FString::Printf(TEXT("Online System: %s"), *SubsystemName));

        SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
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
        ServerCreateDelegate.Broadcast(false);
        return;
    }

    // Destruímos uma sessão se ela já existe
    FNamedOnlineSession *ExistingSession = SessionInterface->GetNamedSession(MySessionName);
    if (ExistingSession)
    {
        PrintString(FString::Printf(TEXT("Session %s already exists, destroying it..."), *MySessionName.ToString()));
        bCreateServerAfterDestroy = true;
        DestroyedServerName = ServerName;

        SessionInterface->DestroySession(MySessionName);
        // Retornamos porque DestroySession é async
        return;
    }

    // Settings
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.NumPublicConnections = 16;

    // vv Funções da Steam vv
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinViaPresence = true;
    // ^^ Funções da Steam ^^

    // Cria sessão como LAN se não conseguir conectar com a Steam
    SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

    SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
    if (ServerName.IsEmpty())
    {
        PrintString("Serve name cannot be empty.");
        ServerJoinDelegate.Broadcast(false);
        return;
    }
    ServerNameToFind = ServerName;

    // Settings
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    SessionSearch->MaxSearchResults = 9999;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    ServerCreateDelegate.Broadcast(bWasSuccessful);

    if (bWasSuccessful)
    {        
        if (MapPath.IsEmpty())
        {
            MapPath = "/Game/ThirdPerson/Maps/ThirdPersonMap";
        }

        FString Path = FString::Printf(TEXT("%s?listen"), *MapPath);
        GetWorld()->ServerTravel(Path);
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
    ServerJoinDelegate.Broadcast(bWasSuccessful);

    if (!bWasSuccessful || ServerNameToFind.IsEmpty() || !SessionSearch)
    {
        return;
    }

    TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
    FOnlineSessionSearchResult *CorrectResult = 0;

    if (Results.Num() > 0)
    {
        FString Message = FString::Printf(TEXT("%d sessions found."), Results.Num());
        PrintString(Message);

        for (FOnlineSessionSearchResult Result : Results)
        {
            if (Result.IsValid())
            {
                FString ServerName = "No-name";
                Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

                if (ServerName.Equals(ServerNameToFind))
                {
                    CorrectResult = &Result;
                    PrintString(FString::Printf(TEXT("Found server with name %s"), *ServerName));
                    break;
                }
            }
        }

        if (CorrectResult)
        {
            SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
        }
        else
        {
            PrintString(FString::Printf(TEXT("Couldn't find server with name %s"), *ServerNameToFind));
            ServerJoinDelegate.Broadcast(false);
            ServerNameToFind = "";
        }
    }
    else
    {
        PrintString("Zero sessions found.");
        ServerJoinDelegate.Broadcast(false);
    }
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    bool bWasSuccessful = Result == EOnJoinSessionCompleteResult::Success;
    ServerJoinDelegate.Broadcast(bWasSuccessful);

    if (bWasSuccessful)
    {
        PrintString(FString::Printf(TEXT("Successfully joined session %s"), *SessionName.ToString()));
        
        FString Address = "";
        bool Success = SessionInterface->GetResolvedConnectString(SessionName, Address);
        if(Success)
        {
            PrintString(FString::Printf(TEXT("Address: %s"), *Address));
            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }
        }
        else
        {
            PrintString("GetResolvedConnectString returned false.");
            ServerJoinDelegate.Broadcast(false);
        }
    }
    else
    {
        PrintString("OnJoinSessionComplete failed.");
        ServerJoinDelegate.Broadcast(false);
    }
}
