#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectableKeyHolder.generated.h"

UCLASS()
class MULTIPLAYERCOURSE_API ACollectableKeyHolder : public AActor
{
	GENERATED_BODY()
	
public:	
	ACollectableKeyHolder();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* KeyMesh;

	
public:	
	virtual void Tick(float DeltaTime) override;
	
	void ActivateKeyMesh();

private:

	UPROPERTY(EditAnywhere)
	float RotationSpeed;

};
