// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TransporterComponent.h"
#include "PressurePlate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeactivated);

UCLASS()
class MULTIPLAYERCOURSE_API APressurePlate : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APressurePlate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USceneComponent *RootComp;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent *TriggerMesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent *Mesh;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UTransporterComponent* Transporter;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FOnActivated OnActivatedDelegate;
	FOnDeactivated OnDeactivatedDelegate;

	UFUNCTION(BlueprintCallable)
	bool IsActivated() { return Activated; }
	UFUNCTION(BlueprintCallable)
	void SetActivated(bool IsActivated) { Activated = IsActivated; }

private:
	bool Activated;

};
