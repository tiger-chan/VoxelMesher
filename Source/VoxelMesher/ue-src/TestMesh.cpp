
#include "TestMesh.h"
#include "weaver/mesher.hpp"

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
	return std::make_tuple(v, d);
}

static std::tuple<std::vector<int32_t>, tc::vector3i> big_block(size_t size)
{
	int32_t s{ static_cast<int32_t>(size) };
	std::vector<int32_t> result{};
	result.resize(size * size * size, 1);

	return std::make_tuple(result, tc::vector3i{ s, s, s });
}

static std::tuple<std::vector<int32_t>, tc::vector3i> hill(size_t size)
{
	auto s = static_cast<int32_t>(size);
	return make_voxels(tc::vector3i{ -s, -s, 0 }, tc::vector3i{ s, s, s },
			   [s](auto i, auto j, auto k) {
				   return j <= s * exp(-(i * i + k * k) / 64.0);
			   });
}

static std::tuple<std::vector<int32_t>, tc::vector3i> valley(size_t size)
{
	auto s = static_cast<int32_t>(size);
	return make_voxels(tc::vector3i{ 0, 0, 0 }, tc::vector3i{ s, s, s },
			   [s](auto i, auto j, auto k) {
				   return j <= (i * i + k * k) * 31.0 / (s * s * 2) + 1;
			   });
}

static std::tuple<std::vector<int32_t>, tc::vector3i> build_shape(ETestShape shape, size_t size = 1)
{
	switch (shape) {
	default:
	case ETestShape::Block:
		return big_block(size);
	case ETestShape::Hill:
		return hill(size);
	case ETestShape::Valley:
		return valley(size);
	}
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

	auto &&[block, dimensions] = build_shape(shape, block_size);

	switch (mesher_type) {
	case EMesherType::Simple: {
		tc::simple<int32_t> mesher{ dimensions.x, dimensions.y, dimensions.z, true };
		result = mesher.eval(std::begin(block), std::end(block));
	} break;

	case EMesherType::Culling: {
		tc::culling<int32_t> mesher{ dimensions.x, dimensions.y, dimensions.z, true };
		result = mesher.eval(std::begin(block), std::end(block));
	} break;
	}
}

void ATestMesh::Step()
{
	++step;
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
	for (auto k = 0; k < result.quads.size() && k < step; ++k) {
		const auto &quad = result.quads[k];
		auto first = vertices.Num();
		auto uv = std::begin(quad.uv);

		for (auto q : quad) {
			auto &vert = q;
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
		for (auto &i :
		     quad.get_triange(tc::quad::triangle::first, tc::quad::rotation::cw)) {
			triangles.Emplace(first + i);
		}

		for (auto &i :
		     quad.get_triange(tc::quad::triangle::second, tc::quad::rotation::cw)) {
			triangles.Emplace(first + i);
		}
	}

	// vertexColors.Init(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), vertices.Num());
	// tangents.Init(FProcMeshTangent(1.0f, 0.0f, 0.0f), vertices.Num());

	//Function that creates mesh section
	procedural_mesh->ClearAllMeshSections();
	procedural_mesh->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs,
						       vertexColors, tangents, false);

	procedural_mesh->SetMaterial(0, material);
}

void ATestMesh::CompleteSteps()
{
	step = result.quads.size();
	Step();
}
