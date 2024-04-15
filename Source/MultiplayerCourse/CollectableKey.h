#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectableKey.generated.h"

class UCapsuleComponent;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCollectableKeyOnCollected);

UCLASS()
class MULTIPLAYERCOURSE_API ACollectableKey : public AActor
{
	GENERATED_BODY()
	
public:	
	ACollectableKey();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
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
	virtual void Tick(float DeltaTime) override;

	FCollectableKeyOnCollected OnCollected;

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
