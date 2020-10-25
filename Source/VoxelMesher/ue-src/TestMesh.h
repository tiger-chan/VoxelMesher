#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "procedural_voxel_mesh_component.h"
#include "Materials/MaterialInterface.h"
#include "weaver/mesher.hpp"
#include "nlohmann/json.hpp"
#include "weaver/core/voxel_def.hpp"
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

			std::vector<voxel_face_result> operator()(const int32_t &, voxel_face vf) const
			{
				std::vector<voxel_face_result> results;
				if (!def) {
					return results;
				}
				
				for (auto&& component: def->components) {
					auto face = component.faces.find(vf);
					if (face == std::end(component.faces)) {
						continue;
					}

					results.emplace_back(voxel_face_result{
						component.min,
						component.max,
						component.translate,
						face->second
					});
				}

				return results;
			}

			tc::voxel_def* def{nullptr};
		};
	}
}

UENUM(BlueprintType)
enum class ETestShape : uint8
{
	Block,
	Hill,
	Valley,
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Block)
	USceneComponent *scene_root{ nullptr };

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Block)
	UProceduralVoxelMeshComponent *procedural_mesh{ nullptr };

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Block)
	UMaterialInterface* material{nullptr};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Block)
	ETestShape shape;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Block)
	EMesherType mesher_type;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Block)
	int32 block_size;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Block)
	FVector block_min;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Block)
	FVector block_max;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Block)
	FVector translate;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Block)
	bool cull_face;

	UFUNCTION(BlueprintCallable)
	void ReloadBlockDef();

	void run_mesher();

	UPROPERTY(EditAnywhere, meta = (RelativePath, RelativeToGameContentDir), Category = Block)
	FDirectoryPath block_def_dir{ };

	UPROPERTY(EditAnywhere, Category = Block)
	FString drawn_block_name{ };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Block)
	class UDataTable* block_material_mapping;

	TMap<uint32_t, FName> block_ids;

	tc::mesher_result result;
	int32_t step{0};

	std::unordered_map<std::string, nlohmann::json> raw_block_json;
	std::unordered_map<std::string, tc::voxel_def> voxel_definitions;
	tc::weaver::voxel_reader<int32_t> reader;
};
