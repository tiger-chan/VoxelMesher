#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "TestMesh.generated.h"

UENUM(BlueprintType)
enum class ETestShape : uint8
{
	Block,
	Hill,
	Valley
};

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

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ETestShape shape;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 block_size;
};
