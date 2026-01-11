// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterVehicle.generated.h"

UCLASS()
class VEHICLETEST_API ACharacterVehicle : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterVehicle();

	//ACharacterVehicle(const FObjectInitializer& ObjectInitializer);

	TObjectPtr<class UChaosVehicleMovementComponent> GetVehicleMoveComp() { return m_vehicleMovementComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** vehicle simulation component */
	UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UChaosVehicleMovementComponent> m_vehicleMovementComponent;

	USceneComponent* m_customRoot;
};
