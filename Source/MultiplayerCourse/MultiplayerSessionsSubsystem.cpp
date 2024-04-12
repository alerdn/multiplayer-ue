// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"


void PrintString(const FString& Message)
{
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, Message);
    }
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();    
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
            PrintString("Session interface is valid.");
        }
    }
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
}
