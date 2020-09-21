
#include "TestMesh.h"
#include "lib-voxel-mesher/simple_voxel_mesher.hpp"

namespace
{
	static std::vector<int32_t> big_block(size_t size)
	{
		std::vector<int32_t> result{};
		result.resize(size * size * size, 1);

		return result;
	}
}

ATestMesh::ATestMesh()
{
	scene_root = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(scene_root);

	procedural_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	procedural_mesh->bCastDynamicShadow = false;
	procedural_mesh->bUseAsyncCooking = true;
	procedural_mesh->AttachToComponent(
		RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void ATestMesh::BeginPlay()
{
	Super::BeginPlay();

	tc::simple_voxel_mesher mesher{};
	static constexpr size_t size = 1;
	auto block = big_block(size);
	auto result = mesher.eval(std::begin(block), std::end(block), size, size, size);

	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertexColors;
	TArray<FProcMeshTangent> tangents;

	vertices.Reserve(result.vertices.size());
	normals.Reserve(result.vertices.size());
	uvs.Reserve(result.vertices.size());
	triangles.Reserve(result.quads.size() * 6);

	auto& verts = result.vertices;
	for (auto& quad : result.quads)
	{
		auto first = vertices.Num();
		auto uv = std::begin(quad.uv);
		for (auto q : quad)
		{
			auto& vert = verts[q];
			vertices.Emplace(
			FVector{
				static_cast<float>(vert.x) * 100.0f - 50.0f,
				static_cast<float>(vert.y) * 100.0f - 50.0f,
				static_cast<float>(vert.z) * 100.0f - 50.0f
			}
			);
			auto& n = quad.normal;
			normals.Emplace(FVector{
				static_cast<float>(n.x),
				static_cast<float>(n.y),
				static_cast<float>(n.z)
			});
			uvs.Emplace(FVector2D{
				static_cast<float>(uv->x),
				static_cast<float>(uv->y)
			});
			++uv;
		}

		// use offset from "first" for the index of triangles.
		for (auto i = 0; i < 3; ++i) {
			auto j = (3 - i)  % 3;
			triangles.Emplace(first + j);
		}

		for (auto i = 0; i < 3; ++i) {
			auto j = (4 - i)  % 4;
			triangles.Emplace(first + j);
		}
	}

	// uvs.Init(FVector2D(0.0f, 0.0f), vertices.Num());
	// vertexColors.Init(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), vertices.Num());
	// tangents.Init(FProcMeshTangent(1.0f, 0.0f, 0.0f), vertices.Num());

	//Function that creates mesh section
	procedural_mesh->CreateMeshSection_LinearColor(
		0,
		vertices,
		triangles,
		normals,
		uvs,
		vertexColors,
		tangents,
		false
		);

	procedural_mesh->SetMaterial(0, material);
}
