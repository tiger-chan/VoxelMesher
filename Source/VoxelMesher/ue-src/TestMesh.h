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

			std::vector<voxel_def> operator()(const int32_t &, voxel_face vf) const
			{
				switch (vf) {
					case voxel_face::back:
					case voxel_face::bottom: {
						return std::vector<voxel_def>{
							voxel_def{min, max, translation, cull_face}
						};
					};

					case voxel_face::right:
					case voxel_face::left: {
						vector3d step_one {
							max.x,
							max.y,
							max.z * 0.5,
						};
						vector2d uv_step_one_min{ 0.0, 0.0 };
						vector2d uv_step_one_max { 1.0, 0.5 };

						vector3d step_two {
							min.x,
							max.y * 0.5,
							max.z * 0.5,
						};

						vector2d uv_step_two_min{ 0.5, 0.5 };
						vector2d uv_step_two_max { 1.0, 1.0 };

						return std::vector<voxel_def>{
							voxel_def{min, step_one, translation, cull_face, uv_step_one_min, uv_step_one_max},
							voxel_def{step_two, max, translation, cull_face, uv_step_two_min, uv_step_two_max}
						};
					}
					case voxel_face::front:
					case voxel_face::top: {
						vector3d step_one {
							max.x,
							max.y * 0.5,
							max.z * 0.5,
						};

						vector3d step_two {
							0.0,
							max.y * 0.5,
							max.z * 0.5,
						};

						return std::vector<voxel_def>{
							voxel_def{min, step_one, translation, cull_face},
							voxel_def{step_two, max, translation, cull_face}
						};
					};

					default: {
						return std::vector<voxel_def>{
						};
					}
				}
			}

			vector3d min{0.0, 0.0, 0.0};
			vector3d max{1.0, 1.0, 1.0};
			vector3d translation{ 0.0, 0.0, 0.0 };
			bool cull_face{false};
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

UENUM(BlueprintType)
enum class EMesherType : uint8
{
	Simple,
	Culling,
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
	EMesherType mesher_type;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 block_size;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector block_min;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector block_max;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector translate;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool cull_face;

	UFUNCTION(BlueprintCallable)
	void Step();

	UFUNCTION(BlueprintCallable)
	void CompleteSteps();

	tc::mesher_result result;
	int32_t step{0};
};
