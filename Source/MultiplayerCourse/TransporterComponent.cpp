// Fill out your copyright notice in the Description page of Project Settings.


#include "TransporterComponent.h"
#include "PressurePlate.h"

UTransporterComponent::UTransporterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Função para ativar replication em Actor Component
	SetIsReplicatedByDefault(true);

	MoveTime = 3.f;
	ActivatedTriggerCount = 0;

	bArePointsSet = false;
	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();
}

void UTransporterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bOwnerIsTriggerActor)
	{
		TriggerActors.Add(GetOwner());
	}

	for(AActor *TriggerActor : TriggerActors)
	{
		APressurePlate* PressurePlate = Cast<APressurePlate>(TriggerActor);
		if (PressurePlate)
		{
			PressurePlate->OnActivatedDelegate.AddDynamic(this, &UTransporterComponent::OnPressurePlateActivated);
			PressurePlate->OnDeactivatedDelegate.AddDynamic(this, &UTransporterComponent::OnPressurePlateDeactivated);
		}
	}
}

void UTransporterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(TriggerActors.Num() > 0)
	{
		bAllTriggerActorsTriggered = ActivatedTriggerCount >= TriggerActors.Num();
	}

	AActor* Owner = GetOwner();
	if (Owner && Owner->HasAuthority() && bArePointsSet)
	{
		FVector CurrentLocation = Owner->GetActorLocation();
		float Speed = FVector::Distance(StartPoint, EndPoint) / MoveTime;

		FVector TargetLocation = bAllTriggerActorsTriggered ? EndPoint : StartPoint;
		if (!CurrentLocation.Equals(TargetLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
			Owner->SetActorLocation(NewLocation);
		}
	}
}

void UTransporterComponent::SetPoints(FVector Point1, FVector Point2)
{
	if (Point1.Equals(Point2))
	{
		return;
	}

	StartPoint = Point1;
	EndPoint = Point2;
	bArePointsSet = true;
}

void UTransporterComponent::OnPressurePlateActivated()
{
	ActivatedTriggerCount++;
}

void UTransporterComponent::OnPressurePlateDeactivated()
{
	ActivatedTriggerCount--;
}