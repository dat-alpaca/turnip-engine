#pragma once
#include <tiny_gltf.h>
#include "assets/asset.hpp"
#include "assets/texture/texture_asset.hpp"
#include "graphics/objects/buffer.hpp"

namespace tur
{
	struct MeshData
    {
        std::vector<byte> vertices;
        std::vector<byte> indices;
		BufferIndexType indexType;
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
		FIELD_SET_INTERNALLY MeshData meshData;
		FIELD_SET_INTERNALLY MetallicRoughnessMaterialData meshMaterial;
	};
}