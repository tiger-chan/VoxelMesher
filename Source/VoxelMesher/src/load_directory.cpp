#include "load_directory.hpp"
#include "Interfaces/IPluginManager.h"
#include <string>
#include "Misc/Paths.h"
#include "weaver/core/voxel_loader.hpp"

namespace VoxelMesher {
	tc::weaver::voxel_load_result load_directory(const FDirectoryPath &path)
	{
		auto p = path.Path;
		if (FPaths::IsRelative(p))
		{
			p = FPaths::Combine(FPaths::ProjectContentDir(), p);
		}
 
		std::string directory{ TCHAR_TO_UTF8(*p) };
		return tc::weaver::load_voxels(directory);
	}
}
