// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

void PrintString(const FString &Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, Message);
    }
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
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
    SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() != "Steam";

    SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if(bWasSuccessful)
    {
        GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
    }
}
