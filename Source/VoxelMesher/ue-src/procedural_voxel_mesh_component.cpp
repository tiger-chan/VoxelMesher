#include "procedural_voxel_mesh_component.h"
#include "lib-voxel-mesher/mesher_result.hpp"


FVoxelMeshData UProceduralVoxelMeshComponent::generate_meshdata(const tc::mesher_result& result)
{
	FVoxelMeshData r;
	auto& vertices = r.vertices;
	auto& normals = r.normals;
	auto& uvs = r.uvs;
	auto& triangles = r.triangles;

	vertices.Reserve(result.vertices.size());
	normals.Reserve(result.vertices.size());
	uvs.Reserve(result.vertices.size());
	triangles.Reserve(result.quads.size() * 6);

	auto &verts = result.vertices;
	for (auto &quad : result.quads) {
		auto first = vertices.Num();
		auto uv = std::begin(quad.uv);
		for (auto q : quad) {
			auto &vert = verts[q];
			vertices.Emplace(FVector{ static_cast<float>(vert.x) * 100.0f - 50.0f,
						  static_cast<float>(vert.y) * 100.0f - 50.0f,
						  static_cast<float>(vert.z) * 100.0f - 50.0f });
			auto &n = quad.normal;
			normals.Emplace(FVector{ static_cast<float>(n.x), static_cast<float>(n.y),
						 static_cast<float>(n.z) });
			uvs.Emplace(
				FVector2D{ static_cast<float>(uv->x), static_cast<float>(uv->y) });
			++uv;
		}

		// use offset from "first" for the index of triangles.
		for (auto i = 0; i < 3; ++i) {
			auto j = (3 - i) % 3;
			triangles.Emplace(first + j);
		}

		for (auto i = 0; i < 3; ++i) {
			auto j = (4 - i) % 4;
			triangles.Emplace(first + j);
		}
	}

	return r;
}