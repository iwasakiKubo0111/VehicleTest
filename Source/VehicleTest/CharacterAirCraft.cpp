// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAirCraft.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/KismetSystemLibrary.h>

 //Sets default values
ACharacterAirCraft::ACharacterAirCraft()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true; // ˆÚ“®•ûŒü‚É‰ñ“]
    GetCharacterMovement()->RotationRate = FRotator(720.f, 720.f, 720.f); // ‰ñ“]‘¬“x‚ð’²®
}

void ACharacterAirCraft::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    //SetRootComponent(GetMesh());
    //GetCapsuleComponent()->DestroyComponent();
}

// Called when the game starts or when spawned
void ACharacterAirCraft::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ACharacterAirCraft::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterAirCraft::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}