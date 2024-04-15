#include "CollectableKeyHolder.h"

ACollectableKeyHolder::ACollectableKeyHolder()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));

	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	KeyMesh->SetupAttachment(RootComp);
	KeyMesh->SetIsReplicated(true);
	KeyMesh->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

void ACollectableKeyHolder::BeginPlay()
{
	Super::BeginPlay();
	
	KeyMesh->SetVisibility(false);
}

void ACollectableKeyHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		KeyMesh->AddRelativeRotation(FRotator(.0f, RotationSpeed * DeltaTime, .0f));
	}
}

void ACollectableKeyHolder::ActivateKeyMesh()
{
	KeyMesh->SetVisibility(true);
}

