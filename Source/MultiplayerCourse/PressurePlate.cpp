// Fill out your copyright notice in the Description page of Project Settings.

#include "PressurePlate.h"

// Sets default values
APressurePlate::APressurePlate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(RootComp);
	TriggerMesh->SetIsReplicated(true);

	auto TriggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));
	if (TriggerMeshAsset.Succeeded())
	{
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(3.3f, 3.3f, .2f));
		TriggerMesh->SetRelativeLocation(FVector(.0f, .0f, 10.f));
	}

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08.SM_building_part_08"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(4.f, 4.f, .5f));
		Mesh->SetRelativeLocation(FVector(.0f, .0f, 7.2f));
	}

	Transporter = CreateDefaultSubobject<UTransporterComponent>(TEXT("Transporter"));
	Transporter->SetMoveTime(.25f);
	Transporter->SetOwnerIsTriggerActor(true);

	Activated = false;
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));

	FVector Point1 = GetActorLocation();
	FVector Point2 = Point1 + FVector(.0f, .0f, -10.f);
	Transporter->SetPoints(Point1, Point2);
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TArray<AActor *> Actors;
		AActor *TriggerActor = 0;
		TriggerMesh->GetOverlappingActors(Actors);

		for (AActor *Actor : Actors)
		{
			if (Actor->ActorHasTag("TriggerActor"))
			{
				TriggerActor = Actor;
				break;
			}
		}

		if (TriggerActor)
		{
			if (!Activated)
			{
				Activated = true;
				OnActivatedDelegate.Broadcast();
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, "Activated");
			}
		}
		else
		{
			if (Activated)
			{
				Activated = false;
				OnDeactivatedDelegate.Broadcast();
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, "Deactivated");
			}
		}
	}
}
