#pragma once
#include <string>
#include <tiny_gltf.h>
#include <vector>
#include <glm/glm.hpp>
#include "common.hpp"

namespace
{
    inline tur::u64 get_component_size(int componentType)
    {
        switch (componentType) {
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

    inline tur::u64 get_component_count(int type)
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
}

namespace tur
{
    struct MeshData
    {
        std::vector<byte> vertices;
        std::vector<byte> indices;
    };

    inline MeshData extract_mesh_data(const tinygltf::Model& model)
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

    struct MaterialInfo
    {
        std::string albedoFilepath;
        int magFilter, minFilter, wrapS, wrapT;
    };

    inline MaterialInfo extract_texture_data(const tinygltf::Model& model)
    {
        MaterialInfo materialInfo;

        const auto& mesh = model.meshes.front();

        for (const auto& primitive : mesh.primitives)
        {
            const auto& material = model.materials[primitive.material];
            
            auto albedoTextureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
            int albedoSourceIndex = model.textures[albedoTextureIndex].source;
            auto albedoTextureFilepath = model.images[albedoSourceIndex].uri;
            materialInfo.albedoFilepath = albedoTextureFilepath; 
            
            int albedoSamplerIndex = model.textures[albedoTextureIndex].sampler;
            materialInfo.magFilter = model.samplers[albedoSamplerIndex].magFilter;
            materialInfo.minFilter =model.samplers[albedoSamplerIndex].minFilter;
            materialInfo.wrapS =model.samplers[albedoSamplerIndex].wrapS;
            materialInfo.wrapT =model.samplers[albedoSamplerIndex].wrapT;
        }

        return materialInfo;
    }
}