#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "weaver/mesher.hpp"
#include "TestMesh.generated.h"

namespace tc
{
	namespace weaver
	{
		template<>
		struct voxel_reader<int32_t>
		{
			bool visible(const int32_t& v) const
			{
				return !(v == 0 || v == unset_voxel_id);
			}

			voxel_id_t operator()(const int32_t& v) const
			{
				return v;
			}
		};
	}
}

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

	UFUNCTION(BlueprintCallable)
	void Step();

	UFUNCTION(BlueprintCallable)
	void CompleteSteps();

	tc::mesher_result result;
	int32_t step{0};
};
