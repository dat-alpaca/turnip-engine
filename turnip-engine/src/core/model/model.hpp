#pragma once
#include <tiny_gltf.h>
#include <vector>
#include "core/defines.hpp"

namespace tur
{
    struct ModelData
    {
        std::vector<unsigned char> vertices;
        std::vector<unsigned char> indices;
    };

    inline ModelData extract_model_data(const tinygltf::Model& model)
    {
        ModelData modelData;

        for (const auto& mesh : model.meshes)
        {
            for (const auto& primitive : mesh.primitives)
            {
                int positionAccessorIndex = primitive.attributes.at("POSITION");
                int normalAccessorIndex = primitive.attributes.at("NORMAL");
                int uvAccessorIndex  = primitive.attributes.at("TEXCOORD_0");
                int indicesAccessorIndex = primitive.indices;

                const auto& positionAccessor = model.accessors[positionAccessorIndex];
                const auto& normalAccessor = model.accessors[normalAccessorIndex];
                const auto& uvAccessor  = model.accessors[uvAccessorIndex];
                const auto& indicesAccessor = model.accessors[indicesAccessorIndex];

                const auto& positionBufferView = model.bufferViews[positionAccessor.bufferView];
                const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
                const auto& uvBufferView  = model.bufferViews[uvAccessor.bufferView];
                const auto& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];

                const unsigned char* positionData =
                    model.buffers[positionBufferView.buffer].data.data() +
                    positionBufferView.byteOffset + positionAccessor.byteOffset;

                const unsigned char* normalData =
                    model.buffers[normalBufferView.buffer].data.data() +
                    normalBufferView.byteOffset + normalAccessor.byteOffset;

                const unsigned char* uvData =
                    model.buffers[uvBufferView.buffer].data.data() +
                    uvBufferView.byteOffset + uvAccessor.byteOffset;

                const unsigned char* indicesData =
                    model.buffers[indicesBufferView.buffer].data.data() +
                    indicesBufferView.byteOffset + indicesAccessor.byteOffset;

                auto readIndex = [&](u32 index) -> u32
                {
                    if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                        return reinterpret_cast<const u16*>(indicesData)[index];

                    return reinterpret_cast<const u32*>(indicesData)[index];
                };

                u32 indexCount = indicesAccessor.count;
                u32 vertexStride = sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2);

                modelData.vertices.reserve(modelData.vertices.size() + indexCount * vertexStride);
                modelData.indices.reserve(modelData.indices.size() + indexCount);

                u32 baseIndex = static_cast<u32>(modelData.indices.size());

                for (u32 i = 0; i < indexCount; ++i)
                {
                    u32 idx = readIndex(i);

                    modelData.vertices.insert(
                        modelData.vertices.end(),
                        positionData + idx * sizeof(glm::vec3),
                        positionData + (idx + 1) * sizeof(glm::vec3));

                    modelData.vertices.insert(
                        modelData.vertices.end(),
                        normalData + idx * sizeof(glm::vec3),
                        normalData + (idx + 1) * sizeof(glm::vec3));

                    modelData.vertices.insert(
                        modelData.vertices.end(),
                        uvData + idx * sizeof(glm::vec2),
                        uvData + (idx + 1) * sizeof(glm::vec2));

                    modelData.indices.push_back(baseIndex + i);
                }
            }
        }

        return modelData;
    }
}