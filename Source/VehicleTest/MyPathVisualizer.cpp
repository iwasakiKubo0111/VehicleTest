#include "MyPathVisualizer.h"
#include "Components/SplineMeshComponent.h"
#include "Components/BillboardComponent.h"

AMyPathVisualizer::AMyPathVisualizer()
{
    PrimaryActorTick.bCanEverTick = false;

    // SplineComponent を作成
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
    RootComponent = SplineComponent;
}

void AMyPathVisualizer::BeginPlay()
{
    Super::BeginPlay();
}

void AMyPathVisualizer::SetPathPoints(const TArray<FVector>& Points)
{
    if (!SplineComponent) return;

    SplineComponent->ClearSplinePoints(false);

    for (int32 Index = 0; Index < Points.Num(); ++Index)
    {
        // SplinePoint を追加
        SplineComponent->AddSplinePoint(Points[Index], ESplineCoordinateSpace::World, false);

        // それぞれのポイントにスプライト（点）を追加
        CreatePointSprite(Points[Index]);

        //SplineComponent->SetTangentAtSplinePoint(Index, FVector::ZeroVector, ESplineCoordinateSpace::Local);
        //SplineComponent->SetSplinePointType(Index, ESplinePointType::Linear);
    }

    //https://www.reddit.com/r/unrealengine/comments/1jd8yy6/how_can_i_smooth_a_spline_fix_tangents_at_runtime/
    for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
    {
        FVector Prev = SplineComponent->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::Local);
        FVector Curr = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
        FVector Next = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

        FVector Dir = Next - Prev;
        if (!Dir.IsNearlyZero())
        {
            FVector NormDir = Dir.GetSafeNormal();

            float LenPrev = (Curr - Prev).Size();
            float LenNext = (Next - Curr).Size();

            FVector ArriveTangent = NormDir * (LenPrev/ 1.0f);//1.0fを上げると直線的になる
            FVector LeaveTangent = NormDir * (LenNext / 1.0f);

            SplineComponent->SetTangentsAtSplinePoint(i, ArriveTangent, LeaveTangent, ESplineCoordinateSpace::Local, false);
        }
    }

    SplineComponent->UpdateSpline();
}

void AMyPathVisualizer::CreatePointSprite(const FVector& WorldLocation)
{
    // BillboardComponent をスポーンして Spline にアタッチ
    UBillboardComponent* SpriteComp = NewObject<UBillboardComponent>(this);

    if (SpriteComp)
    {
        SpriteComp->RegisterComponent();
        SpriteComp->SetWorldLocation(WorldLocation);
        SpriteComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

        // マテリアルがあればセット（丸点表示など）
        if (SpriteMaterial)
        {
            SpriteComp->SetMaterial(0, SpriteMaterial);
        }

        // 大きさ調整（任意）
        SpriteComp->SetRelativeScale3D(FVector(0.2f));
    }
}
