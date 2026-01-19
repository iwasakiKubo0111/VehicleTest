// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterShip.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/KismetSystemLibrary.h>

 //Sets default values
ACharacterShip::ACharacterShip()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SetRootComponent(GetMesh());
    GetCapsuleComponent()->DestroyComponent();

}

// Called when the game starts or when spawned
void ACharacterShip::BeginPlay()
{
    Super::BeginPlay();

    //GetCapsuleComponent();



    LogActorBoundsSecond();

    // 2秒後に2回目の出力を予約
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_BoundsSecond,
        this,
        &ACharacterShip::LogActorBoundsSecond,
        2.0f,  // Delay (秒)
        false  // ループしない
    );

}

// Called every frame
void ACharacterShip::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterShip::LogActorBoundsSecond()
{
    FVector Origin;
    FVector BoxExtent;
    GetActorBounds(true, Origin, BoxExtent);


    // Origin のログ文字列
    FString OriginMsg = FString::Printf(TEXT("Bounds Origin: X=%.2f Y=%.2f Z=%.2f"),
        Origin.X, Origin.Y, Origin.Z);

    // BoxExtent のログ文字列
    FString ExtentMsg = FString::Printf(TEXT("Box Extent: X=%.2f Y=%.2f Z=%.2f"),
        BoxExtent.X, BoxExtent.Y, BoxExtent.Z);

    // PrintString で表示
    UKismetSystemLibrary::PrintString(this, OriginMsg, true, true, FLinearColor::Green, 180.0f);
    UKismetSystemLibrary::PrintString(this, ExtentMsg, true, true, FLinearColor::Yellow, 180.0f);

    // ワールド上にボックスを描画
    DrawDebugBox(
        GetWorld(),
        Origin,
        BoxExtent,
        FQuat::Identity,             // 回転が不要なら Identity
        FColor::Red,                 // 色
        false,                       // persist (残すか)
        180.0f,                        // 表示時間
        0,                           // Depth Priority
        5.0f                        // 線の太さ
    );
}

void ACharacterShip::GetActorBounds(bool bOnlyCollidingComponents, FVector& Origin, FVector& BoxExtent, bool bIncludeFromChildActors)const
{
    if (GetMesh())
    {
        FTransform ActorTrans = GetMesh()->GetComponentTransform();
        const FBoxSphereBounds LocalBounds = GetMesh()->CalcLocalBounds();

        FBoxSphereBounds WorldBounds = LocalBounds.TransformBy(ActorTrans);

        Origin = WorldBounds.Origin;
        BoxExtent = WorldBounds.BoxExtent;

        //UE_LOG(LogTemp, Log, TEXT("Mesh Local Bounds Origin=(%.2f %.2f %.2f) Extent=(%.2f %.2f %.2f)"),
        //    LocalOrigin.X, LocalOrigin.Y, LocalOrigin.Z,
        //    LocalExtent.X, LocalExtent.Y, LocalExtent.Z);
    }
}



