#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "TestMesh.generated.h"

UCLASS(BlueprintType)
class VOXELMESHER_API ATestMesh : public AActor {
	GENERATED_BODY()
    public:
	ATestMesh();

	protected:
	void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USceneComponent *scene_root{ nullptr };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UProceduralMeshComponent *procedural_mesh{ nullptr };

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UMaterialInterface* material{nullptr};
};
