// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerCourseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/StaticMeshActor.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DamageEvents.h"

//////////////////////////////////////////////////////////////////////////
// AMultiplayerCourseCharacter

AMultiplayerCourseCharacter::AMultiplayerCourseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			 // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;		// The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;								// Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	OriginalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MaxRunSpeed = 1500.f;
}

void AMultiplayerCourseCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerCourseCharacter::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerCourseCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerCourseCharacter::Look);

		// Run
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AMultiplayerCourseCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AMultiplayerCourseCharacter::Run);

		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AMultiplayerCourseCharacter::Shoot);
	}
}

void AMultiplayerCourseCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMultiplayerCourseCharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMultiplayerCourseCharacter::ToggleRunSpeed(bool bRunning)
{
	if (bRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = OriginalMaxWalkSpeed;
	}
}

void AMultiplayerCourseCharacter::Run(const FInputActionValue &Value)
{
	bool bRunning = Value.Get<bool>();
	if (HasAuthority())
	{
		ToggleRunSpeed(bRunning);
	}
	else
	{
		ServerRPCRun(bRunning);
	}
}

void AMultiplayerCourseCharacter::ServerRPCRun_Implementation(bool bRunning)
{
	ToggleRunSpeed(bRunning);
}

void AMultiplayerCourseCharacter::TryShoot()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		FVector ViewPointLocation;
		FRotator ViewPointRotation;
		PlayerController->GetPlayerViewPoint(ViewPointLocation, ViewPointRotation);

		FHitResult Hit;
		FVector Start = ViewPointLocation;
		FVector End = ViewPointLocation + ViewPointRotation.Vector() * 1000.f;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		bool HasHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, Params);
		if (HasHit)
		{
			float Damage = 50.f;
			FPointDamageEvent DamageEvent(Damage, Hit, -ViewPointRotation.Vector(), nullptr);
			AActor* HitActor = Hit.GetActor();
			HitActor->TakeDamage(Damage, DamageEvent, GetController(), this);
		}
	}
}

void AMultiplayerCourseCharacter::Shoot()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("Player %d shot"), GPlayInEditorID));
	if (HasAuthority())
	{
		// Se sou servidor posso atirar
		TryShoot();
	} else
	{
		// Se não sou servidor, peço ao servidor para atirar
		ServerRPCTryShoot();
	}
	
}

void AMultiplayerCourseCharacter::ServerRPCTryShoot_Implementation()
{
	TryShoot();
}

float AMultiplayerCourseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
 	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(DamageToApply, 100.f);

	Destroy();

	return DamageToApply;
}

// Esta função não é declarada no header
// No header declaramos ServerRPCFunction e o cliente sabe que tem que
// executar a versão Implementation
void AMultiplayerCourseCharacter::ServerRPCFunction_Implementation(int id)
{
	if (HasAuthority())
	{
		if (!SphereMesh)
		{
			return;
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		AStaticMeshActor *StaticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnParameters);
		if (StaticMeshActor)
		{
			StaticMeshActor->SetReplicates(true);
			StaticMeshActor->SetReplicateMovement(true);
			StaticMeshActor->SetMobility(EComponentMobility::Movable);

			FVector SpawnLocation = GetActorLocation() + GetActorRotation().Vector() * 100.f + GetActorUpVector() * 50.f;
			StaticMeshActor->SetActorLocation(SpawnLocation);

			UStaticMeshComponent *StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
			if (StaticMeshComponent)
			{
				StaticMeshComponent->SetIsReplicated(true);
				StaticMeshComponent->SetSimulatePhysics(true);
				StaticMeshComponent->SetStaticMesh(SphereMesh);
			}
		}
	}
}

// Esta função é executada antes de ServerRPCFunction_Implementation
// Se ela retornar true, a função Implementation é executada, senão
// o player é expulso do jogo
bool AMultiplayerCourseCharacter::ServerRPCFunction_Validate(int id)
{
	// Simulando validação onde id só pode ser entre 0 e 100
	return id >= 0 && id <= 100;
}

void AMultiplayerCourseCharacter::ClientRPCFunction_Implementation()
{
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	}
}