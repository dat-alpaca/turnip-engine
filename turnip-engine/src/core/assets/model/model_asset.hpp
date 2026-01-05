#pragma once
#include "assets/asset.hpp"
#include <tiny_gltf.h>

namespace tur
{
	struct ModelAsset : public Asset
	{
	public:
		ModelAsset() { metadata.type = AssetType::MODEL; }

	public:
		tinygltf::Model model;
	};
}