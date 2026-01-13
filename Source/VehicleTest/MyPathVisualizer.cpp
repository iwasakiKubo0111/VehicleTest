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

//void AMyPathVisualizer::SetPathPoints(const TArray<FVector>& Points)
//{
//    if (!SplineComponent) return;
//
//    SplineComponent->ClearSplinePoints(false);
//
//    for (int32 Index = 0; Index < Points.Num(); ++Index)
//    {
//        // SplinePoint を追加
//        SplineComponent->AddSplinePoint(Points[Index], ESplineCoordinateSpace::World, false);
//
//        // それぞれのポイントにスプライト（点）を追加
//        CreatePointSprite(Points[Index]);
//
//        //SplineComponent->SetTangentAtSplinePoint(Index, FVector::ZeroVector, ESplineCoordinateSpace::Local);
//        //SplineComponent->SetSplinePointType(Index, ESplinePointType::Linear);
//    }
//
//    //https://www.reddit.com/r/unrealengine/comments/1jd8yy6/how_can_i_smooth_a_spline_fix_tangents_at_runtime/
//    for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
//    {
//        FVector Prev = SplineComponent->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::Local);
//        FVector Curr = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
//        FVector Next = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
//
//        FVector Dir = Next - Prev;
//        if (!Dir.IsNearlyZero())
//        {
//            FVector NormDir = Dir.GetSafeNormal();
//
//            float LenPrev = (Curr - Prev).Size();
//            float LenNext = (Next - Curr).Size();
//
//            FVector ArriveTangent = NormDir * (LenPrev/ 1.0f);//1.0fを上げると直線的になる
//            FVector LeaveTangent = NormDir * (LenNext / 1.0f);
//
//            SplineComponent->SetTangentsAtSplinePoint(i, ArriveTangent, LeaveTangent, ESplineCoordinateSpace::Local, false);
//        }
//    }
//
//    SplineComponent->UpdateSpline();
//}

void AMyPathVisualizer::SetPathPoints(const TArray<FVector>& Points, float Offset /* = 0.0f */)
{
    if (!SplineComponent) return;

    SplineComponent->ClearSplinePoints(false);

    const int32 NumPoints = Points.Num();
    if (NumPoints == 0)
    {
        SplineComponent->UpdateSpline();
        return;
    }

    // 1) スプライン点を追加（途中点はオフセットする）
    for (int32 Index = 0; Index < NumPoints; ++Index)
    {
        FVector AddPoint = Points[Index];

        // 始点と終点はオフセットしない
        if (Index != 0 && Index != NumPoints - 1 && Offset != 0.0f)
        {
            const FVector PrevP = Points[Index - 1];
            const FVector NextP = Points[Index + 1];

            FVector TangentDir = (NextP - PrevP);
            if (!TangentDir.IsNearlyZero())
            {
                TangentDir = TangentDir.GetSafeNormal();

                // 横方向（Lateral）を求める：Tangent x Up
                FVector Lateral = FVector::CrossProduct(TangentDir, FVector::UpVector).GetSafeNormal();

                // もし接線がほぼ上下方向で Up と平行になってしまう場合のフォールバック
                if (Lateral.IsNearlyZero())
                {
                    // 代替として RightVector を使う（任意）
                    Lateral = FVector::CrossProduct(TangentDir, FVector::RightVector).GetSafeNormal();
                    if (Lateral.IsNearlyZero())
                    {
                        // 最終手段：任意の横方向
                        Lateral = FVector::RightVector;
                    }
                }

                // 「外側」の符号を決める（中点からの方向に対して同じ向きなら +1、逆なら -1）
                const FVector Mid = (PrevP + NextP) * 0.5f;
                float sign = FVector::DotProduct(Points[Index] - Mid, Lateral);

                float s = (sign >= 0.0f) ? 1.0f : -1.0f;

                // オフセット適用
                AddPoint = Points[Index] + Lateral * (Offset * s);
            }
        }

        DrawDebugSphere(GetWorld(), AddPoint, 20.0f, 12, FColor::Blue, false, 300.0f);
        SplineComponent->AddSplinePoint(AddPoint, ESplineCoordinateSpace::World, false);
    }

    // 2) 既存のタンジェント調整処理（安全のためインデックスをクランプ）
    const int32 NumSplinePoints = SplineComponent->GetNumberOfSplinePoints();
    for (int32 i = 0; i < NumSplinePoints; ++i)
    {
        int32 PrevIdx = FMath::Clamp(i - 1, 0, NumSplinePoints - 1);
        int32 NextIdx = FMath::Clamp(i + 1, 0, NumSplinePoints - 1);

        FVector Prev = SplineComponent->GetLocationAtSplinePoint(PrevIdx, ESplineCoordinateSpace::Local);
        FVector Curr = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
        FVector Next = SplineComponent->GetLocationAtSplinePoint(NextIdx, ESplineCoordinateSpace::Local);

        FVector Dir = Next - Prev;
        if (!Dir.IsNearlyZero())
        {
            FVector NormDir = Dir.GetSafeNormal();

            float LenPrev = (Curr - Prev).Size();
            float LenNext = (Next - Curr).Size();

            // 1.0f を大きくするとより直線的に（元コードのまま）
            FVector ArriveTangent = NormDir * (LenPrev / 1.0f);
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
