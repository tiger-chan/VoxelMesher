
#include "TestMesh.h"
#include "weaver/mesher.hpp"
#include "load_directory.hpp"

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

	procedural_mesh = CreateDefaultSubobject<UProceduralVoxelMeshComponent>(TEXT("ProceduralMesh"));
	procedural_mesh->bCastDynamicShadow = false;
	procedural_mesh->bUseAsyncCooking = true;
	procedural_mesh->AttachToComponent(RootComponent,
					   FAttachmentTransformRules::KeepRelativeTransform);
}

void ATestMesh::BeginPlay()
{
	Super::BeginPlay();

	ReloadBlockDef();
}

void ATestMesh::run_mesher()
{
	auto &&[block, dimensions] = build_shape(shape, block_size);

	procedural_mesh->block_material_mapping = block_material_mapping;
	procedural_mesh->block_name_map = &block_ids;

	switch (mesher_type) {
	case EMesherType::Simple: {
		tc::simple<int32_t> mesher{ dimensions.x, dimensions.y, dimensions.z, true };
		procedural_mesh->draw_voxels(std::begin(block), std::end(block), mesher, reader);
	} break;

	case EMesherType::Culling: {
		tc::culling<int32_t> mesher{ dimensions.x, dimensions.y, dimensions.z, true };
		result = mesher.eval(std::begin(block), std::end(block), reader);
		procedural_mesh->draw_voxels(std::begin(block), std::end(block), mesher, reader);
	} break;
	}
}

void ATestMesh::ReloadBlockDef()
{
	if (!block_def_dir.Path.IsEmpty()) {
		auto &&[ raw_blocks, voxels ] = VoxelMesher::load_directory(block_def_dir);
		raw_block_json = raw_blocks;
		voxel_definitions = voxels;
		if (!drawn_block_name.IsEmpty()) {
			std::string name = TCHAR_TO_UTF8(*drawn_block_name);
			auto voxel = voxel_definitions.find(name);
			reader.def = voxel == std::end(voxel_definitions) ? nullptr : &voxel->second;

			block_ids.Empty();
			block_ids.Emplace(0, *drawn_block_name);
			block_ids.Emplace(1, *drawn_block_name);
		}
	}

	run_mesher();
}
