#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "weaver/mesher/fwd.hpp"

#include "procedural_voxel_mesh_component.generated.h"

struct FVoxelMeshData {
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertex_colors;
	TArray<FProcMeshTangent> tangents;
};

UCLASS(BlueprintType)
class VOXELMESHER_API UProceduralVoxelMeshComponent : public UProceduralMeshComponent {
	GENERATED_BODY()

    public:
	template <typename Iter, typename Mesher>
	void draw_voxels(Iter begin, Iter end, Mesher &mesher, bool create_collision = false)
	{
		auto result =
			mesher.eval(std::forward<Iter>(begin), std::forward<Iter>(end));
		auto voxel_data = generate_meshdata(result);

		CreateMeshSection_LinearColor(
			0, voxel_data.vertices, voxel_data.triangles, voxel_data.normals,
			voxel_data.uvs, voxel_data.vertex_colors, voxel_data.tangents,
			create_collision);
	}

    private:
	FVoxelMeshData generate_meshdata(const tc::mesher_result &mesher_result);
};
