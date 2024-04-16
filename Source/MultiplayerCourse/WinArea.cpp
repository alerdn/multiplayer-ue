#include "WinArea.h"
#include "MultiplayerCourseCharacter.h"
#include "Components/BoxComponent.h"

AWinArea::AWinArea()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	WinAreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WinAreaBox"));
	SetRootComponent(WinAreaBox);

	bWinCondition = false;
}

void AWinArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWinArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (!bWinCondition)
		{
			TArray<AActor*> Actors;
			WinAreaBox->GetOverlappingActors(Actors, AMultiplayerCourseCharacter::StaticClass());

			bWinCondition = Actors.Num() == 2;
			if (bWinCondition)
			{
				MulticastRPCWin();
			}
		}

		
	}
}

void AWinArea::MulticastRPCWin_Implementation()
{
	OnWinCondition.Broadcast();
}

