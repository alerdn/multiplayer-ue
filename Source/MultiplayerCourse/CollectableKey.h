// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectableKey.generated.h"

class UCapsuleComponent;
class UAudioComponent;

UCLASS()
class MULTIPLAYERCOURSE_API ACollectableKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectableKey();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	class ACollectableKeyHolder* KeyHolderRef;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	float RotationSpeed;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* CollectAudio;

	UPROPERTY(ReplicatedUsing = OnRep_IsCollected, VisibleAnywhere)
	bool IsCollected;

	UFUNCTION()
	void OnRep_IsCollected(); 

};
