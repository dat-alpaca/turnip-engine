#include "model_loader.hpp"
#include "assets/texture/texture_asset.hpp"
#include "common.hpp"
#include "graphics/objects/texture.hpp"

static tur::u64 get_component_size(int componentType)
{
    switch (componentType) 
    {
        case TINYGLTF_COMPONENT_TYPE_BYTE:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            return 1;

        case TINYGLTF_COMPONENT_TYPE_SHORT:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            return 2;

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            return 4;

        default:
            TUR_LOG_CRITICAL("Invalid tinygltf component type");
    }
}

static constexpr tur::u64 get_component_count(int type)
{
    if(type == TINYGLTF_TYPE_VEC2)
        return 2;

    if(type == TINYGLTF_TYPE_VEC3)
        return 3;

    if(type == TINYGLTF_TYPE_VEC4)
        return 4;

    if(type == TINYGLTF_TYPE_SCALAR)
        return 1;

    TUR_LOG_CRITICAL("Unsupported tinygltf type");
    return 1;
}

static constexpr tur::FilterMode get_filter_mode(int mode, bool isMinFilter)
{
    using namespace tur;
	if (isMinFilter)
	{
		switch (mode)
		{
            case 0:
				return FilterMode::NEAREST;

            case 1:
				return FilterMode::LINEAR;

            case 2:
				return FilterMode::NEAREST;

            case 3:
				return FilterMode::BILINEAR;

			case 4:
				return FilterMode::TRILINEAR;
		}
	}
	else
	{
		switch (mode)
		{
			case 0:
				return FilterMode::NEAREST;

			case 1:
				return FilterMode::LINEAR;
		}
	}

	TUR_LOG_ERROR("Invalid Model Texture Filter Mode: {}. Default: NEAREST", mode);
	return FilterMode::NEAREST;
}

static constexpr tur::WrapMode get_wrap_mode(int mode)
{
    using namespace tur;
    switch(mode)
    {
        case 0:
            return WrapMode::CLAMP_TO_EDGE;
        case 1:
            return WrapMode::MIRRORED_REPEAT;
        case 2:
            return WrapMode::REPEAT;
    }

	TUR_LOG_ERROR("Invalid Model Texture Wrap Mode: {}. Default: REPEAT", mode);
    return WrapMode::REPEAT;
}

namespace tur 
{
    std::optional<tinygltf::Model> load_model_task(const std::filesystem::path& filepath)
    {
        if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load model: '{}'. File does not exist.", filepath.string());
			return std::nullopt;
		}

        tinygltf::Model model;
        tinygltf::TinyGLTF loader;

        std::string error;
        std::string warning;
        bool returnValue = loader.LoadASCIIFromFile(&model, &error, &warning, filepath.string());

        if (!warning.empty())
            TUR_LOG_WARN("", warning);

        if (!error.empty())
        {
            TUR_LOG_ERROR("Failed to load: {}. {}", filepath.string(), error);
            return std::nullopt;
        }

        if(!returnValue)
        {
            TUR_LOG_ERROR("Failed to parse: {}", filepath.string());   
            return std::nullopt;
        }
        
        return model;
    }

    MeshData extract_mesh_data(const tinygltf::Model& model)
    {
        using namespace tinygltf;
        MeshData modelData;

        const auto& mesh = model.meshes.front();

        auto get_data = [](const Model& model, const Primitive& primitive, int attributeIndex) -> std::vector<byte> {
            const auto& accessor = model.accessors[attributeIndex];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer  = model.buffers[bufferView.buffer];

            u64 componentSize = get_component_size(accessor.componentType); // sizeof(float) for VEC3f
            u64 componentCount = get_component_count(accessor.type); // 3 for VEC3f

            size_t elementSize = componentSize * componentCount;
            size_t stride = bufferView.byteStride ? bufferView.byteStride : elementSize;

            std::vector<byte> result(accessor.count * elementSize);
            const byte* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

            for (size_t i = 0; i < accessor.count; ++i)
                std::memcpy(result.data() + i * elementSize, data + i * stride, elementSize);

            return result;
        };

        for (const auto& primitive : mesh.primitives)
        {
            // Indices:
            auto indicesData = get_data(model, primitive, primitive.indices);

            // Data:
            auto positionData = get_data(model, primitive, primitive.attributes.at("POSITION"));
            auto normalData = get_data(model, primitive, primitive.attributes.at("NORMAL"));
            auto uvData = get_data(model, primitive, primitive.attributes.at("TEXCOORD_0"));

            // Interleaving:
            u64 vertexCount = model.accessors[primitive.attributes.at("POSITION")].count;
            u64 stride = 8 * sizeof(float);
            
            std::vector<byte> vertices(stride * vertexCount);
            byte* destination = vertices.data();

            for (size_t i = 0; i < vertexCount; ++i)
            {
                std::memcpy(destination, positionData.data() + i * sizeof(glm::vec3), sizeof(glm::vec3)); 
                destination += sizeof(glm::vec3);

                std::memcpy(destination, normalData.data() + i * sizeof(glm::vec3), sizeof(glm::vec3)); 
                destination += sizeof(glm::vec3);

                std::memcpy(destination, uvData.data() + i * sizeof(glm::vec2), sizeof(glm::vec2)); 
                destination += sizeof(glm::vec2);
            }

            modelData.vertices = vertices;
            modelData.indices = indicesData;
        }

        return modelData;
    }

    MetallicRoughnessMaterial extract_texture_data(const tinygltf::Model& model)
    {
        MetallicRoughnessMaterial materialInfo;

        const auto& mesh = model.meshes.front();

        for (const auto& primitive : mesh.primitives)
        {
            const auto& material = model.materials[primitive.material];
            const auto& pbr = material.pbrMetallicRoughness;

            // Base color:
            materialInfo.baseColorFactor = glm::vec4(1.0f);
            if (pbr.baseColorFactor.size() == 4)
            {
                materialInfo.baseColorFactor = {
                    static_cast<float>(pbr.baseColorFactor[0]),
                    static_cast<float>(pbr.baseColorFactor[1]),
                    static_cast<float>(pbr.baseColorFactor[2]),
                    static_cast<float>(pbr.baseColorFactor[3]),
                };
            }

            // Albedo:
            int baseColorTextureIndex = pbr.baseColorTexture.index;
            if(baseColorTextureIndex >= 0)
            {
                const auto& texture = model.textures[baseColorTextureIndex];
                const auto& image   = model.images[texture.source];
                const auto& sampler = model.samplers[texture.sampler];

                TextureAsset asset;
                {
                    asset.data = image.image;
                    asset.width = image.width;
                    asset.height = image.height;

                    asset.options.minFilter = get_filter_mode(sampler.minFilter, true);
                    asset.options.magFilter = get_filter_mode(sampler.magFilter, false);

                    asset.options.wrapS = get_wrap_mode(sampler.wrapS);
                    asset.options.wrapT = get_wrap_mode(sampler.wrapT);
                }

                materialInfo.albedoTexture = asset;
            }
        }

        return materialInfo;
    }
}