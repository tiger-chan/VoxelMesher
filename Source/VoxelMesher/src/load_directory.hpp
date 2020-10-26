#ifndef MINERVA_RECURSIVE_JSON_LOADER_HPP
#define MINERVA_RECURSIVE_JSON_LOADER_HPP

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/EngineTypes.h"
#include "nlohmann/json.hpp"
#include "weaver/core/voxel_def.hpp"
#include "weaver/core/voxel_loader.hpp"
#include <string_view>

namespace VoxelMesher
{
	tc::weaver::voxel_load_result VOXELMESHER_API load_directory(const FDirectoryPath &path);
}

#endif // MINERVA_RECURSIVE_JSON_LOADER_HPP
