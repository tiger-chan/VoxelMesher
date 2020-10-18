#include "procedural_voxel_mesh_component.h"
#include "weaver/mesher/mesher_result.hpp"


TArray<FVoxelMeshData> UProceduralVoxelMeshComponent::generate_meshdata(const tc::mesher_result& result)
{
	TArray<FVoxelMeshData> results;
	std::unordered_map<tc::weaver::voxel_id_t, uint32_t> voxel_types;
	const auto alloc_vert_size = (result.quads.size() * 4) / 2;
	const auto alloc_quad_size = (result.quads.size() * 6) / 2;

	auto get_mesh_data = [&results, &voxel_types, alloc_vert_size, alloc_quad_size](tc::weaver::voxel_id_t id) -> FVoxelMeshData& {
		auto data = voxel_types.find(id);
		if (data == std::end(voxel_types)) {
			voxel_types.emplace(id, results.Num());
			FVoxelMeshData& r = results.Emplace_GetRef();
			r.voxel_type = id;
			r.vertices.Reserve(alloc_vert_size);
			r.normals.Reserve(alloc_vert_size);
			r.uvs.Reserve(alloc_vert_size);
			r.triangles.Reserve(alloc_quad_size);
			return r;
		}

		return results[data->second];
	};

	auto &verts = result.vertices;
	for (auto k = 0; k < result.quads.size(); ++k) {
		const auto &quad = result.quads[k];
		auto type_id = quad.type_id == tc::weaver::unset_voxel_id ? 0 : quad.type_id;
		auto& r = get_mesh_data(type_id);
		auto first = r.vertices.Num();
		auto uv = std::begin(quad.uv);

		for (auto q : quad) {
			auto &vert = q;
			r.vertices.Emplace(FVector{ static_cast<float>(vert.x) * 100.0f - 50.0f,
						  static_cast<float>(vert.y) * 100.0f - 50.0f,
						  static_cast<float>(vert.z) * 100.0f - 50.0f });
			auto &n = quad.normal;
			r.normals.Emplace(FVector{ static_cast<float>(n.x), static_cast<float>(n.y),
						 static_cast<float>(n.z) });
			r.uvs.Emplace(
				FVector2D{ static_cast<float>(uv->x), static_cast<float>(uv->y) });
			++uv;
		}

		// use offset from "first" for the index of triangles.
		for (auto& i : quad.get_triange(tc::quad::triangle::first, tc::quad::rotation::cw)) {
			r.triangles.Emplace(first + i);
		}

		for (auto& i : quad.get_triange(tc::quad::triangle::second, tc::quad::rotation::cw)) {
			r.triangles.Emplace(first + i);
		}
	}

	return results;
}