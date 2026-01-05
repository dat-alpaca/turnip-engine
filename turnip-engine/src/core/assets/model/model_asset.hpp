#pragma once
#include <tiny_gltf.h>
#include "assets/asset.hpp"
#include "assets/texture/texture_asset.hpp"

namespace tur
{
	struct MeshData
    {
        std::vector<byte> vertices;
        std::vector<byte> indices;
    };

	struct MetallicRoughnessMaterial
    {
		glm::vec4 baseColorFactor;
		TextureAsset albedoTexture;
    };

	struct ModelAsset : public Asset
	{
	public:
		ModelAsset() { metadata.type = AssetType::MODEL; }

	public:
		tinygltf::Model model; // todo: remove
		MeshData meshData; // todo: one per mesh
		MetallicRoughnessMaterial meshMaterial; // todo: one per mesh
	};
}