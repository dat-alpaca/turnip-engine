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

	struct MetallicRoughnessMaterialData
    {
		glm::vec4 baseColorFactor;
		TextureAsset albedoTexture;
		TextureAsset normalTexture;
		TextureAsset metallicTexture;
		TextureAsset roughnessTexture;
    };

	struct MeshAsset : public Asset
	{
	public:
		MeshAsset() { metadata.type = AssetType::MESH; }

	public:
		MeshData meshData;
		MetallicRoughnessMaterialData meshMaterial;
	};
}