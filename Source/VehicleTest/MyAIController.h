// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

class UChaosVehicleMovementComponent;
class USplineComponent;
class ACharacterVehicle;
class AMyPathVisualizer;

UENUM(BlueprintType)  // Blueprint ‚ÉŒöŠJ
enum class EModelAIState : uint8
{
	IDLE = 0,
	GENARATE_SPLINE,
	MOVE,
};

/**
 * 
 */
UCLASS()
class VEHICLETEST_API AMyAIController : public AAIController
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void StartMove();
	void Move(float DeltaSeconds);

	float CalcSteeringInput(USplineComponent* spline);

	float CalcBreakeInput(USplineComponent* spline, FVector location);

	void VehicleSpeedControl();

	float GetPercentSplineLocation(USplineComponent* spline,FVector location);

	FTimerHandle DelayTimerHandle;

	EModelAIState m_modelAIState = EModelAIState::IDLE;

	UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UChaosVehicleMovementComponent> m_owningVehicleComp = NULL;

	UPROPERTY()
	ACharacterVehicle* m_owningCharacterVehicle = NULL;

	UPROPERTY()
	AMyPathVisualizer* m_owningSplineActor = NULL;

	float m_defaultMaxSpeed = 0;
	float m_maxSpeed = 20;
	float m_minSpeed = 10;
	float m_yawDelta = 0;
	float m_targetSpeed = 0;
	float m_maxWheelSpinRotation = 0;
	bool m_isForward = true;
	float m_goalPercent = 0;
	FVector m_offsetTangentLocation;
};
