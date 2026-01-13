// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include <NavigationSystem.h>
#include "NavigationPath.h"
#include "MyPathVisualizer.h"
#include "CharacterVehicle.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Components/SplineComponent.h"

void AMyAIController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("Called AMyAIController::BeginPlay"));

	GetWorld()->GetTimerManager().SetTimer(
		DelayTimerHandle,
		this,
		&AMyAIController::StartMove,
		3.0f,
		false);

	m_defaultMaxSpeed = m_maxSpeed;
	ACharacterVehicle* charaVehicle = Cast<ACharacterVehicle>(GetPawn());
	m_owningCharacterVehicle = charaVehicle;
	m_owningVehicleComp = m_owningCharacterVehicle->GetVehicleMoveComp();

	UChaosWheeledVehicleMovementComponent* wheeledComp = Cast<UChaosWheeledVehicleMovementComponent>(m_owningVehicleComp);

	const FChaosWheelSetup& setup = wheeledComp->WheelSetups[0];
	UChaosVehicleWheel* defaultWheelCDO = setup.WheelClass.GetDefaultObject();
	if (defaultWheelCDO)
	{
		m_maxWheelSpinRotation = defaultWheelCDO->MaxWheelspinRotation;
		UE_LOG(LogTemp, Log, TEXT("MaxWheelspinRotation: %f"), m_maxWheelSpinRotation);
	}
}

void AMyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch (m_modelAIState)
	{
	case EModelAIState::IDLE:
		break;
	case EModelAIState::GENARATE_SPLINE:
		//
		break;
	case EModelAIState::MOVE:
		Move(DeltaSeconds);
		break;

	default:
		break;
	}
}

void AMyAIController::StartMove()
{
	APawn* MyPawn = GetPawn();
	FVector StartLocation = MyPawn->GetActorLocation();
	FVector EndLocation = FVector(-3800.0, -3530.0, 0.0); // 目的地

	// Navigation System を取得
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys == nullptr) return;

	UE_LOG(LogTemp, Log, TEXT("NavSys Valid"));

	// 経路を計算
	UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(
		GetWorld(),
		StartLocation,
		EndLocation,
		this
	);

	TArray<FVector> Points;

	if (NavPath && NavPath->IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("NavPath Valid"));

		// PathPoints をログ出力など
		for (const FNavPathPoint& Point : NavPath->PathPoints)
		{
			Points.Add(Point);
			DrawDebugSphere(GetWorld(), Point, 20.0f, 12, FColor::Red, false, 300.0f);
			UE_LOG(LogTemp, Log, TEXT("Nav Point: %s"), *Point.Location.ToString());
		}

		AMyPathVisualizer* Viz = GetWorld()->SpawnActor<AMyPathVisualizer>(AMyPathVisualizer::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

		if (Viz)
		{
			Viz->SetPathPoints(Points);
			m_owningSplineActor = Viz;

			// 途中到達判定とする場合の場所
			FVector goal(-10.0, -1160.0, 0.0f);
			m_goalPercent = GetPercentSplineLocation(m_owningSplineActor->GetSplineComponent(), goal);
		}

		m_modelAIState = EModelAIState::MOVE;
	}
}

void AMyAIController::Move(float DeltaSeconds)
{
	if (m_owningSplineActor)
	{
		// 到達判定
		float currentPercent = GetPercentSplineLocation(m_owningSplineActor->GetSplineComponent(), GetPawn()->GetActorLocation());
		if (currentPercent >= m_goalPercent)
		{
			m_modelAIState = EModelAIState::IDLE;
		}

		float steering = CalcSteeringInput(m_owningSplineActor->GetSplineComponent());
		m_owningVehicleComp->SetSteeringInput(steering);

		VehicleSpeedControl();
	}
}

float AMyAIController::CalcSteeringInput(USplineComponent* spline)
{
	FVector closestTangent = spline->FindTangentClosestToWorldLocation(m_owningCharacterVehicle->GetActorLocation(),ESplineCoordinateSpace::World);
	FVector normarizeVec = closestTangent.GetSafeNormal(0.0001f);

	FVector multiplieredVec = normarizeVec * (m_owningVehicleComp->GetForwardSpeedMPH() * 5 + UKismetMathLibrary::SelectFloat(250.0f, 350.0f, m_isForward));
	FVector shouldBeInputVectorFromMyLocation = m_owningCharacterVehicle->GetActorLocation() + multiplieredVec;

	m_offsetTangentLocation = spline->FindLocationClosestToWorldLocation(shouldBeInputVectorFromMyLocation, ESplineCoordinateSpace::World);

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(m_owningCharacterVehicle->GetActorLocation(), m_offsetTangentLocation);
	FRotator actorRotRolled180ForBack = UKismetMathLibrary::ComposeRotators(m_owningCharacterVehicle->GetActorRotation(), FRotator(0, -180, 0));
	FRotator selectedRotation = UKismetMathLibrary::SelectRotator(m_owningCharacterVehicle->GetActorRotation(), actorRotRolled180ForBack,m_isForward);
	FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRotation, selectedRotation);
	m_yawDelta = delta.Yaw;

	float clampedYawDelta = UKismetMathLibrary::MapRangeClamped(
		m_yawDelta,
		-15,
		15,
		-1,
		1
	);

	float result = UKismetMathLibrary::SelectFloat(clampedYawDelta, clampedYawDelta * -1, m_isForward);

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		m_offsetTangentLocation, // 中心位置
		25,               // 半径
		12,                 // セグメント数
		FColor::Green,      // 色
		false,              // 永続描画
		0.1f                 // 表示時間（秒）
	);

	UKismetSystemLibrary::DrawDebugArrow(
		this,                         // WorldContextObject（通常 this）
		m_owningCharacterVehicle->GetActorLocation(),               // 始点
		m_offsetTangentLocation,             // 終点
		1.f,                         // 矢印サイズ
		FLinearColor::Green,            // 色
		0,                         // 表示時間（秒）
		10.0f                          // 線の太さ
	);
#endif

	return result;
}

float AMyAIController::CalcBreakeInput(USplineComponent* spline, FVector originLocation)
{
	if (FMath::Abs(m_owningVehicleComp->GetForwardSpeedMPH()) <= FMath::Abs(m_minSpeed)) return 0;

	FVector closestTangent = spline->FindTangentClosestToWorldLocation(originLocation,ESplineCoordinateSpace::World);
	FVector normarizeVec = closestTangent.GetSafeNormal(0.0001f);

	float minClampedSpeed = FMath::Clamp(m_owningVehicleComp->GetForwardSpeedMPH(),1.0f, m_owningVehicleComp->GetForwardSpeedMPH());
	float x50Speed = minClampedSpeed * 50.0f;

	FVector projectedLocationAhead = originLocation + (normarizeVec * x50Speed);

	FVector closestSplineLocationToProjected = spline->FindLocationClosestToWorldLocation(projectedLocationAhead, ESplineCoordinateSpace::World);
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(m_owningCharacterVehicle->GetActorLocation(), closestSplineLocationToProjected);
	FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRotation, m_owningCharacterVehicle->GetActorRotation());

	float yawMetric = FMath::Abs(delta.Yaw) + (FMath::Abs(m_yawDelta) / 4);
	float breakeInput = UKismetMathLibrary::MapRangeClamped(
		yawMetric,
		15,
		35,
		0,
		1
	);

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		closestSplineLocationToProjected, // 中心位置
		25,               // 半径
		12,                 // セグメント数
		FColor::Red,      // 色
		false,              // 永続描画
		0.1f                 // 表示時間（秒）
	);

	UKismetSystemLibrary::DrawDebugArrow(
		this,                         // WorldContextObject（通常 this）
		m_owningCharacterVehicle->GetActorLocation(),               // 始点
		closestSplineLocationToProjected,             // 終点
		1.f,                         // 矢印サイズ
		FLinearColor::Red,            // 色
		0,                         // 表示時間（秒）
		10.0f                          // 線の太さ
	);
#endif

	return breakeInput;
}

void AMyAIController::VehicleSpeedControl()
{
	if (m_isForward)
	{
		// 前進

		float yawDeltaAbs = FMath::Abs(m_yawDelta);
		m_targetSpeed = UKismetMathLibrary::MapRangeClamped(
			yawDeltaAbs,
			m_maxWheelSpinRotation,
			5,
			m_minSpeed,
			m_maxSpeed
		);

		if (m_owningVehicleComp->GetForwardSpeedMPH() < m_targetSpeed)
		{
			m_owningVehicleComp->SetThrottleInput(1.0);
		}
		else
		{
			m_owningVehicleComp->SetThrottleInput(-1.0);
		}

		float breakeInput = CalcBreakeInput(m_owningSplineActor->GetSplineComponent(), m_offsetTangentLocation);
		m_owningVehicleComp->SetBrakeInput(breakeInput);
		m_owningVehicleComp->SetHandbrakeInput(false);
	}
	else
	{
		// 後退

		float yawDeltaAbs = FMath::Abs(m_yawDelta);
		m_targetSpeed = UKismetMathLibrary::MapRangeClamped(
			yawDeltaAbs,
			5,
			0,
			m_minSpeed,
			m_maxSpeed * 1.5f
		);

		if (m_owningVehicleComp->GetForwardSpeedMPH() > FMath::Abs(m_targetSpeed) * -1 )
		{
			m_owningVehicleComp->SetBrakeInput(1.0f);
		}
		else
		{
			m_owningVehicleComp->SetBrakeInput(-1.0f);
		}

		m_owningVehicleComp->SetThrottleInput(0);
		m_owningVehicleComp->SetHandbrakeInput(false);
	}
}

float AMyAIController::GetPercentSplineLocation(USplineComponent* spline, FVector location)
{
	
	float InputKey = spline->FindInputKeyClosestToWorldLocation(location);
	//UKismetSystemLibrary::PrintString(
	//	this,                 // WorldContextObject（例: this）
	//	FString::SanitizeFloat(InputKey), // 表示したい文字列
	//	true,                 // 画面に表示するか
	//	true,                 // Output Log に出すか
	//	FLinearColor::Yellow, // 文字色
	//	180.0f                  // 表示時間（秒）
	//);
	float DistanceAlong = spline->GetDistanceAlongSplineAtSplineInputKey(InputKey);
	//UKismetSystemLibrary::PrintString(
	//	this,                 // WorldContextObject（例: this）
	//	FString::SanitizeFloat(DistanceAlong), // 表示したい文字列
	//	true,                 // 画面に表示するか
	//	true,                 // Output Log に出すか
	//	FLinearColor::Blue, // 文字色
	//	180.0f                  // 表示時間（秒）
	//);
	float TotalSplineLength = spline->GetSplineLength();
	//UKismetSystemLibrary::PrintString(
	//	this,                 // WorldContextObject（例: this）
	//	FString::SanitizeFloat(TotalSplineLength), // 表示したい文字列
	//	true,                 // 画面に表示するか
	//	true,                 // Output Log に出すか
	//	FLinearColor::Red, // 文字色
	//	180.0f                  // 表示時間（秒）
	//);
	float PercentAlong = (TotalSplineLength > 0.0f)
		? (DistanceAlong / TotalSplineLength)
		: 0.0f;
	UKismetSystemLibrary::PrintString(
		this,                 // WorldContextObject（例: this）
		FString::SanitizeFloat(PercentAlong), // 表示したい文字列
		true,                 // 画面に表示するか
		true,                 // Output Log に出すか
		FLinearColor::Green, // 文字色
		180.0f                  // 表示時間（秒）
	);
	return PercentAlong;
}


