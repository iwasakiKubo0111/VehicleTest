// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CharacterShip.h"
#include "ShipAIController.generated.h"

class USplineComponent;
class ACharacterVehicle;
class AMyPathVisualizer;

UENUM(BlueprintType)  // Blueprint ‚ÉŒöŠJ
enum class EShipAIState : uint8
{
	IDLE = 0,
	GENARATE_SPLINE,
	MOVE,
};

/**
 *
 */
UCLASS()
class VEHICLETEST_API AShipAIController : public AAIController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void StartMove();
	void Move(float DeltaSeconds);

	float CalcSteeringInput(USplineComponent* spline);

	float CalcBreakeInput(USplineComponent* spline, FVector location);

	void VehicleSpeedControl();

	float GetPercentSplineLocation(USplineComponent* spline, FVector location);

	float GetCurrentSpeedMPH();

	FTimerHandle DelayTimerHandle;

	EShipAIState m_modelAIState = EShipAIState::IDLE;

	UPROPERTY()
	ACharacterShip* m_owningCharacterShip = NULL;

	UPROPERTY()
	USkeletalMeshComponent* m_owningSkelMeshComp = NULL;

	UPROPERTY()
	AMyPathVisualizer* m_owningSplineActor = NULL;

	float m_defaultMaxSpeed = 0;
	float m_maxSpeed = 10;
	float m_minSpeed = 3;
	float m_accelForce = 100000.0f;
	float m_steeringForce = 10000000.f;
	float m_yawDelta = 0;
	float m_targetSpeed = 0;
	float m_maxWheelSpinRotation = 30;
	bool m_isForward = true;
	float m_goalPercent = 0;
	FVector m_offsetTangentLocation;
};
