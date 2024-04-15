#include "MoveableActor.h"
#include "Components/ArrowComponent.h"

AMoveableActor::AMoveableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Point1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point1"));
	Point1->SetupAttachment(RootComp);
	Point1->SetRelativeLocation(FVector::Zero());

	Point2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point2"));
	Point2->SetupAttachment(RootComp);
	Point2->SetRelativeLocation(FVector(.0f, .0f, 300.f));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	Transporter = CreateDefaultSubobject<UTransporterComponent>(TEXT("Transporter"));
}

void AMoveableActor::BeginPlay()
{
	Super::BeginPlay();
	
	FVector StartPoint = GetActorLocation() + Point1->GetRelativeLocation();
	FVector EndPoint = GetActorLocation() + Point2->GetRelativeLocation();

	Transporter->SetPoints(StartPoint, EndPoint);
}

void AMoveableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

