// Fill out your copyright notice in the Description page of Project Settings.

#include "MyBox.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyBox::AMyBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ReplicatedVar = 100.f;

	// A classe precisa ser replicável para uma de suas variáveis ser replicável
	bReplicates = true;
}

// Called when the game starts or when spawned
void AMyBox::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::DecreaseReplicatedVar, 2.f, false);
	}

	// 	if (HasAuthority())
	// 	{
	//* 	Escreve mensagem na tela
	// 		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Server"));
	// 	}else
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("Server"));
	// 	}
}

// Called every frame
void AMyBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Esta função é chamada automaticamente nos cliente
// assim que a variável ReplicatedVar é alterada pelo server
void AMyBox::OnRep_ReplicatedVar()
{
	if (HasAuthority())
	{
		FVector NewLocation = GetActorLocation() + FVector(.0f, .0f, 200.f);
		SetActorLocation(NewLocation);

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Server: OnRep_ReplicatedVar"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Client: %d: OnRep_ReplicatedVar"), GPlayInEditorID));
	}
}

void AMyBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyBox, ReplicatedVar);
}

void AMyBox::DecreaseReplicatedVar()
{
	if (HasAuthority())
	{
		ReplicatedVar--;

		// Precisamos chamar manualmente no server
		OnRep_ReplicatedVar();
		if (ReplicatedVar >0)
		{
			GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::DecreaseReplicatedVar, 2.f, false);
		}
	}
}
