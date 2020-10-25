#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "weaver/config/config.hpp"
#include "weaver/mesher/fwd.hpp"
#include "Engine/DataTable.h"
#include "Materials/MaterialInterface.h"

#include "procedural_voxel_mesh_component.generated.h"

struct FVoxelMeshData {
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertex_colors;
	TArray<FProcMeshTangent> tangents;
	tc::weaver::voxel_id_t voxel_type;
};

USTRUCT(BlueprintType)
struct VOXELMESHER_API FBlockMaterialRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInterface> Material;
};

UCLASS(BlueprintType)
class VOXELMESHER_API UProceduralVoxelMeshComponent : public UProceduralMeshComponent {
	GENERATED_BODY()

    public:
	template <typename Iter, typename Mesher, typename Reader>
	void draw_voxels(Iter begin, Iter end, Mesher &mesher, Reader reader = {}, bool create_collision = false)
	{
		auto result =
			mesher.eval(std::forward<Iter>(begin), std::forward<Iter>(end), reader);
		auto voxel_data = generate_meshdata(result);

		for (auto i = 0; i < voxel_data.Num(); ++i)
		{
			const auto& data = voxel_data[i];
			CreateMeshSection_LinearColor(
				i, data.vertices, data.triangles, data.normals,
			data.uvs, data.vertex_colors, data.tangents,
			create_collision);
			
			auto name = (*block_name_map)[data.voxel_type];
			static const FString context{ TEXT("UProceduralVoxelMeshComponent::draw_voxels") };
			if (auto material_row = block_material_mapping->FindRow<FBlockMaterialRow>(name, context ))
			{
				SetMaterial(i, material_row->Material.LoadSynchronous());
			}
		}
	}

	UDataTable* block_material_mapping{ nullptr };
	TMap<uint32_t, FName>* block_name_map { nullptr };

    private:
	TArray<FVoxelMeshData> generate_meshdata(const tc::mesher_result &mesher_result);
};
