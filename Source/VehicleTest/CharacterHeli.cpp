// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHeli.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterHeli::ACharacterHeli()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; // ˆÚ“®•ûŒü‚É‰ñ“]
	GetCharacterMovement()->RotationRate = FRotator(720.f, 720.f, 720.f); // ‰ñ“]‘¬“x‚ğ’²®
}

// Called when the game starts or when spawned
void ACharacterHeli::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterHeli::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterHeli::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

