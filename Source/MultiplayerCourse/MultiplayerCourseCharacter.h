// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MultiplayerCourseCharacter.generated.h"


UCLASS(config=Game)
class AMultiplayerCourseCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	AMultiplayerCourseCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* SphereMesh;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ExplosionEffect;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Server: Função que é chamada no client e executada no server
	// WithValidation: Espera encontrar uma função _Validate no cpp
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerRPCFunction(int id);

	// Client: Função que é chamada no server e executada no client
	// Reliable: Há garantia que a função será executada
	// Unreliable: Não há garantia que a função será executada.
	// 	Este modelo é útil quando uma função é executada muito frequentemente
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientRPCFunction();

	// Notes:
	// Usar replicated properties para replicar stateful events => pontos de vida
	// Usar multicast/client rpc para replicar transient events ou cosmetic events => criar partícula, ativar tela de sangramento em um player
	// Usar server rpc para se comunicar com o servidor e sempre validar dados => senão pode haver cheaters
};

