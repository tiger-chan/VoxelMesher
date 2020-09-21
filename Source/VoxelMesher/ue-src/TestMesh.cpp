
#include "TestMesh.h"
#include "lib-voxel-mesher/simple_voxel_mesher.hpp"

namespace
{
template <typename Func>
static std::tuple<std::vector<int32_t>, tc::vector3i> make_voxels(tc::vector3i l, tc::vector3i h,
								  Func &&f)
{
	tc::vector3i d{ h[0] - l[0], h[1] - l[1], h[2] - l[2] };
	std::vector<int32_t> v(d[0] * d[1] * d[2]);
	int32_t n = 0;

	for (auto k = l[2]; k < h[2]; ++k) {
		for (auto j = l[1]; j < h[1]; ++j) {
			for (auto i = l[0]; i < h[0]; ++i, ++n) {
				v[n] = f(i, k, j);
			}
		}
	}
	return std::make_tuple( v, d );
}

static std::tuple<std::vector<int32_t>, tc::vector3i> big_block(size_t size)
{
	int32_t s{ static_cast<int32_t>(size) };
	std::vector<int32_t> result{};
	result.resize(size * size * size, 1);

	return std::make_tuple(result, tc::vector3i{ s, s, s });
}

static std::tuple<std::vector<int32_t>, tc::vector3i> hill()
{
	return make_voxels(tc::vector3i{ -16, 0, -16 }, tc::vector3i{ 16, 16, 16 },
			   [](auto i, auto j, auto k) {
				   return j <= 16 * exp(-(i * i + k * k) / 64.0);
			   });
}

static std::tuple<std::vector<int32_t>, tc::vector3i> valley()
{
	return make_voxels(tc::vector3i{ 0, 0, 0 }, tc::vector3i{ 32, 32, 32 },
			   [](auto i, auto j, auto k) {
				   return j <= (i*i + k*k) * 31.0 / (32*32*2) + 1;
			   });
}

} // namespace

ATestMesh::ATestMesh()
{
	scene_root = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(scene_root);

	procedural_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	procedural_mesh->bCastDynamicShadow = false;
	procedural_mesh->bUseAsyncCooking = true;
	procedural_mesh->AttachToComponent(RootComponent,
					   FAttachmentTransformRules::KeepRelativeTransform);
}

void ATestMesh::BeginPlay()
{
	Super::BeginPlay();

	tc::simple_voxel_mesher mesher{};
	static constexpr size_t size = 10;
	auto &&[block, dimensions] = valley(); //big_block(size);
	auto result = mesher.eval(std::begin(block), std::end(block), dimensions.x, dimensions.y,
				  dimensions.z);

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

	// vertexColors.Init(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), vertices.Num());
	// tangents.Init(FProcMeshTangent(1.0f, 0.0f, 0.0f), vertices.Num());

	//Function that creates mesh section
	procedural_mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs,
						       vertexColors, tangents, false);

	procedural_mesh->SetMaterial(0, material);
}
