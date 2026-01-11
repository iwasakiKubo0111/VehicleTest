// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterVehicle.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

 //Sets default values
ACharacterVehicle::ACharacterVehicle()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_vehicleMovementComponent = CreateDefaultSubobject<UChaosVehicleMovementComponent, UChaosWheeledVehicleMovementComponent>("VehicleComp");
	m_vehicleMovementComponent->SetIsReplicated(true); // Enable replication by default
	m_vehicleMovementComponent->UpdatedComponent = GetMesh();

	//GetCapsuleComponent();

    SetRootComponent(GetMesh());

}

//ACharacterVehicle::ACharacterVehicle(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = true;
//	
//	m_vehicleMovementComponent = CreateDefaultSubobject<UChaosVehicleMovementComponent, UChaosWheeledVehicleMovementComponent>("VehicleComp");
//	m_vehicleMovementComponent->SetIsReplicated(true); // Enable replication by default
//}

//ACharacterVehicle::ACharacterVehicle(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer
//		.SetDefaultSubobjectClass<UChaosWheeledVehicleMovementComponent>(ACharacter::CharacterMovementComponentName))
//{
//	PrimaryActorTick.bCanEverTick = true;
//
//	m_vehicleMovementComponent = CreateDefaultSubobject<UChaosVehicleMovementComponent, UChaosWheeledVehicleMovementComponent>("VehicleComp");
//	m_vehicleMovementComponent->SetIsReplicated(true); // Enable replication by default
//}

// Called when the game starts or when spawned
void ACharacterVehicle::BeginPlay()
{
    Super::BeginPlay();


}

// Called every frame
void ACharacterVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

