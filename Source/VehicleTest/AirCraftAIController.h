// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AirCraftAIController.generated.h"

class USplineComponent;
class ACharacterAirCraft;
class ACharacter;
class AMyPathVisualizer;

UENUM(BlueprintType)  // Blueprint ‚ÉŒöŠJ
enum class EAircraftAIState : uint8
{
	IDLE = 0,
	GENARATE_SPLINE,
	MOVE,
};

/**
 * 
 */
UCLASS()
class VEHICLETEST_API AAirCraftAIController : public AAIController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void StartMove();
	void StartMove2();
	void Move(float DeltaSeconds);

	UPROPERTY()
	AMyPathVisualizer* m_owningSplineActor = NULL;

	UPROPERTY()
	APawn* m_owningPawn = NULL;

	FTimerHandle DelayTimerHandle;

	ACharacter* m_owningChara = NULL;

	EAircraftAIState m_modelAIState = EAircraftAIState::IDLE;
	float m_maxSpeed = 2000;
	float m_minSpeed = 10;
	bool m_isForward = true;
	float m_currentSplineLength = 0;

	float m_elapsedTimeChange = 0;

	float m_currentRollAngle = 0;
};
