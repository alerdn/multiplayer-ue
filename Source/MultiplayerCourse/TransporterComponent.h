// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TransporterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERCOURSE_API UTransporterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTransporterComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetPoints(FVector Point1, FVector Point2);

	UFUNCTION(BlueprintCallable)
	bool IsAllTriggerActorsTriggered() { return bAllTriggerActorsTriggered; }
	UFUNCTION(BlueprintCallable)
	void SetAllTriggerActorsTriggered(bool AllTriggerActorsTriggered) { bAllTriggerActorsTriggered = AllTriggerActorsTriggered; }

	UFUNCTION(BlueprintCallable)
	float GetMoveTime() { return MoveTime; }
	UFUNCTION(BlueprintCallable)
	void SetMoveTime(float NewMoveTime) { MoveTime = NewMoveTime; }

	bool IsOwnerTriggerActor() { return bOwnerIsTriggerActor; }
	void SetOwnerIsTriggerActor(bool IsOwnerTriggerActor) { bOwnerIsTriggerActor = IsOwnerTriggerActor; }
	
private:
	FVector StartPoint;		
	FVector EndPoint;

	bool bArePointsSet;

	UPROPERTY(EditAnywhere)
	bool bOwnerIsTriggerActor;

	UPROPERTY(EditAnywhere)
	float MoveTime;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> TriggerActors;
	UPROPERTY(VisibleAnywhere)
	int ActivatedTriggerCount;
	UPROPERTY(VisibleAnywhere)
	bool bAllTriggerActorsTriggered;

	UFUNCTION()
	void OnPressurePlateActivated();
	UFUNCTION()
	void OnPressurePlateDeactivated();
};
