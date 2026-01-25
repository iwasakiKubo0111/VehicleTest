// Fill out your copyright notice in the Description page of Project Settings.


#include "HeliAIController.h"
#include "MyPathVisualizer.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HAL/IConsoleManager.h"

void AHeliAIController::BeginPlay()
{
	Super::BeginPlay();

	/*IConsoleManager::Get().FindConsoleVariable(TEXT("r.ShadowQuality"))->Set(0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicShadows"))->Set(0);*/

	

	m_owningPawn = GetPawn();
	m_owningChara = Cast<ACharacter>(m_owningPawn);
	m_owningChara->GetCharacterMovement()->MaxFlySpeed = m_maxSpeed;

	GetWorld()->GetTimerManager().SetTimer(
		DelayTimerHandle,
		this,
		&AHeliAIController::StartMove,
		3.0f,
		false);

	//SetKeiryoka(true);

	//逆向きにしたい場合
	//USkeletalMeshComponent* MeshComp = m_owningPawn->FindComponentByClass<USkeletalMeshComponent>();
	//if (MeshComp)
	//{
	//	FRotator Rot = MeshComp->GetRelativeRotation();
	//	Rot.Yaw += 180.f;
	//	MeshComp->SetRelativeRotation(Rot);
	//}

}

void AHeliAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch (m_modelAIState)
	{
	case EHeliAIState::IDLE:
		break;
	case EHeliAIState::GENARATE_SPLINE:
		//
		break;
	case EHeliAIState::MOVE:
		Move(DeltaSeconds);
		break;

	default:
		break;
	}
}

void AHeliAIController::StartMove()
{
	FVector EndLocation = FVector(-9940.0, -8510.0, 3430.0); // 目的地

	// 戦闘機の前方位置
	FVector CurrentLocation = m_owningPawn->GetActorLocation();
	FVector ForwardVector = m_owningPawn->GetActorForwardVector();
	FVector ForwardLocation1 = CurrentLocation + (ForwardVector * 500.0f);
	FVector ForwardLocation2 = CurrentLocation + (ForwardVector * 3000.0f);

	FVector ForwardLocation3 = ForwardLocation1;
	ForwardLocation3.Z = (ForwardVector.Z + EndLocation.Z) / 2;

	TArray<FVector> Points;
	Points.Add(CurrentLocation);
	Points.Add(ForwardLocation3);
	Points.Add(EndLocation);

	UE_LOG(LogTemp, Log, TEXT("NavPath Valid"));

	AMyPathVisualizer* Viz = GetWorld()->SpawnActor<AMyPathVisualizer>(AMyPathVisualizer::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	if (Viz)
	{
		Viz->SetPathPoints(Points);
		m_owningSplineActor = Viz;
	}

	m_modelAIState = EHeliAIState::MOVE;
	m_elapsedTimeChange = 0.0f;

}

void AHeliAIController::StartMove2()
{
	FVector EndLocation = FVector(-9930.0, -420.0, 3430.0); // 目的地

	// 戦闘機の前方位置
	FVector CurrentLocation = m_owningPawn->GetActorLocation();
	FVector ForwardVector = m_owningPawn->GetActorForwardVector();
	FVector ForwardLocation1 = CurrentLocation + (ForwardVector * 500.0f);
	FVector ForwardLocation2 = CurrentLocation + (ForwardVector * 3000.0f);


	TArray<FVector> Points;
	Points.Add(CurrentLocation);
	Points.Add(ForwardLocation2);
	Points.Add(EndLocation);

	UE_LOG(LogTemp, Log, TEXT("NavPath Valid"));

	AMyPathVisualizer* Viz = GetWorld()->SpawnActor<AMyPathVisualizer>(AMyPathVisualizer::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	if (Viz)
	{
		Viz->SetPathPoints(Points);
		m_owningSplineActor = Viz;
	}

	m_modelAIState = EHeliAIState::MOVE;
	m_elapsedTimeChange = 0.0f;

}

void AHeliAIController::Move(float DeltaSeconds)
{
	if (m_elapsedTimeChange <= 0)
	{
		if (m_owningSplineActor)
		{
			m_currentSplineLength += m_maxSpeed * DeltaSeconds;

			// 到達判定
			float splineLength = m_owningSplineActor->GetSplineComponent()->GetSplineLength();

			// 終端を超えたら
			if (m_currentSplineLength >= splineLength)
			{
				//m_modelAIState = EAircraftAIState::IDLE;
				StartMove2();
				m_currentSplineLength = 0;
			}

			// スプライン上のターゲット位置
			FVector TargetPos = m_owningSplineActor->GetSplineComponent()->GetLocationAtDistanceAlongSpline(m_currentSplineLength, ESplineCoordinateSpace::World);

			// 方向
			FVector Direction = (TargetPos - m_owningPawn->GetActorLocation()).GetSafeNormal();

			if (Direction.Z > 0.1f)
			{
				m_owningChara->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			}
			else if (Direction.Z < -0.1)
			{
				FVector Start = m_owningChara->GetActorLocation();
				FVector End = Start - FVector(0.f, 0.f, 200.f);

				FHitResult Hit;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(m_owningChara);

				if (bool hit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
				{
#if WITH_EDITOR
					DrawDebugLine(GetWorld(), Start, End, hit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);
#endif
					m_owningChara->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					Direction.Z = 0;
				}
			}

			// スプライン上の位置を設定
			FVector NewLocation = m_owningSplineActor->GetSplineComponent()->GetLocationAtDistanceAlongSpline(m_currentSplineLength, ESplineCoordinateSpace::World);

			// 必要なら向きも設定
			FRotator NewRotation = m_owningSplineActor->GetSplineComponent()->GetRotationAtDistanceAlongSpline(m_currentSplineLength, ESplineCoordinateSpace::World);
			m_owningPawn->SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::None);

			if (m_owningChara->GetCharacterMovement()->MovementMode == MOVE_Flying)
			{
				FVector RightVector = m_owningPawn->GetActorRightVector();
				float SideValue = FVector::DotProduct(Direction.GetSafeNormal(), RightVector);
				SideValue *= 1000;
				float targetRoll = UKismetMathLibrary::MapRangeClamped(
					SideValue,
					-1.000,
					1.000,
					+20,
					-20
				);

				m_currentRollAngle = FMath::FInterpTo(m_currentRollAngle, targetRoll, DeltaSeconds, 0.5f);
				FRotator NewRot = m_owningPawn->GetActorRotation();
				NewRot.Roll = m_currentRollAngle;
				NewRot.Pitch = 0;

				//FRotator CurrentRot = m_owningPawn->GetActorRotation();
				//FRotator TargetRot = m_owningPawn->GetActorRotation();
				//TargetRot.Roll = targetRoll;
				//FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaSeconds, 4);


				// Actor のロールに反映
				m_owningPawn->SetActorRotation(NewRot);

				//UKismetSystemLibrary::PrintString(
				//	this,
				//	FString::Printf(TEXT("Roll: %f"), SideValue),
				//	true,
				//	true,
				//	FLinearColor::Green,
				//	0.f,
				//	FName("Heli") // 同じキーで上書き
				//);
				//UKismetSystemLibrary::PrintString(
				//	this,
				//	FString::Printf(TEXT("targetRoll: %f"), targetRoll),
				//	true,
				//	true,
				//	FLinearColor::Blue,
				//	0.f,
				//	FName("targetRollHeli") // 同じキーで上書き
				//);
			}
		}
	}
	else
	{
		m_elapsedTimeChange -= DeltaSeconds;
		m_elapsedTimeChange = FMath::Clamp(m_elapsedTimeChange, 0, m_elapsedTimeChange);
	}
}

void AHeliAIController::SetKeiryoka(bool isKeiryoka)
{
	int value = 1;

	if (isKeiryoka)
	{
		value = 0;
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ShadowQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicShadows")))
			{
				CVar->Set(value);
			}
		});


	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ContactShadows")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DistanceFieldShadowing")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.HZBOcclusion")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BuildUseCompute")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.ViewDistanceQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.ShadowQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.PostProcessQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.TextureQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.EffectsQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.FoliageQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DepthOfFieldQuality")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientOcclusionLevels")))
			{
				CVar->Set(value);
			}
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick([this,value]()
		{
			if (auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.PostProcessAAQuality")))
			{
				CVar->Set(value);
			}
		});
}