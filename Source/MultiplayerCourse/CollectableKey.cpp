// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableKey.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerCourseCharacter.h"
#include "Components/AudioComponent.h"

ACollectableKey::ACollectableKey()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComp);
	Capsule->SetIsReplicated(true);
	Capsule->SetCollisionProfileName(FName("OverlapAllDynamic"));
	Capsule->SetCapsuleHalfHeight(150.f);
	Capsule->SetCapsuleRadius(100.f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	CollectAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CollectAudio"));
	CollectAudio->SetupAttachment(RootComp);
	CollectAudio->SetAutoActivate(false);

	RotationSpeed = 100.f;
}

void ACollectableKey::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACollectableKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		// Rotaciona a mesh
		Mesh->AddRelativeRotation(FRotator(.0f, RotationSpeed * DeltaTime, .0f));

		TArray<AActor*> OverlappingActors;
		Capsule->GetOverlappingActors(OverlappingActors, AMultiplayerCourseCharacter::StaticClass());

		if(OverlappingActors.Num() > 0 && !IsCollected)
		{
			IsCollected = true;
			// Essa função é chamada pelos clientes automaticamente
			// mas precisa ser invocada manualmente para o servidor
			OnRep_IsCollected();
		}
	}
}

void ACollectableKey::OnRep_IsCollected()
{
	Mesh->SetVisibility(false);
	CollectAudio->Play();
}

void ACollectableKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectableKey, IsCollected);
}
