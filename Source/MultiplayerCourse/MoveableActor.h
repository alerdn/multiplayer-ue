#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TransporterComponent.h"
#include "MoveableActor.generated.h"

class UArrowComponent;

UCLASS()
class MULTIPLAYERCOURSE_API AMoveableActor : public AActor
{
	GENERATED_BODY()

public:
	AMoveableActor();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	USceneComponent *RootComp;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent *Point1;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent *Point2;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Mesh;

	UPROPERTY(VisibleAnywhere)
	UTransporterComponent *Transporter;

public:
	virtual void Tick(float DeltaTime) override;

private:

};
