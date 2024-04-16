#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WinArea.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWinAreaOnWinCondition);

UCLASS()
class MULTIPLAYERCOURSE_API AWinArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AWinArea();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* WinAreaBox;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FWinAreaOnWinCondition OnWinCondition;

private:
	UPROPERTY(VisibleAnywhere)
	bool bWinCondition;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCWin();

};
