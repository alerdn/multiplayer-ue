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

void AMyBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyBox, ReplicatedVar);
}

