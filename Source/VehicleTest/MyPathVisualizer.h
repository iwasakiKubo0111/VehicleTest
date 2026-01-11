#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/BillboardComponent.h"
//#include "NavMeshPath.h"
#include "MyPathVisualizer.generated.h"

UCLASS()
class VEHICLETEST_API AMyPathVisualizer : public AActor
{
    GENERATED_BODY()

public:
    AMyPathVisualizer();

    // 経路ポイント配列をセット
    void SetPathPoints(const TArray<FVector>& Points);
    USplineComponent* GetSplineComponent() { return SplineComponent; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    USplineComponent* SplineComponent;

    UPROPERTY(EditAnywhere)
    UMaterialInterface* SpriteMaterial; // スプライトに使うマテリアル

private:
    void CreatePointSprite(const FVector& WorldLocation);
};
