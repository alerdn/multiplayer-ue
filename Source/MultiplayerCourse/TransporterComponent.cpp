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

	FString Message = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, Message);

}

void UTransporterComponent::OnPressurePlateDeactivated()
{
	ActivatedTriggerCount--;

	FString Message = FString::Printf(TEXT("Transporter Deactivated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, Message);
}