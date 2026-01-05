#pragma once
#include <tiny_gltf.h>
#include "model/model.hpp"
#include "assets/asset.hpp"

namespace tur
{
	struct ModelAsset : public Asset
	{
	public:
		ModelAsset() { metadata.type = AssetType::MODEL; }

	public:
		tinygltf::Model model;
		MeshData meshData; // todo: one per mesh
		// TODO: material data
	};
}