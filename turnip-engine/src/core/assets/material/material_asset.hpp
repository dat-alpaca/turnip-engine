#pragma once
#include <filesystem>
#include "assets/asset.hpp"
#include "graphics/objects/pipeline.hpp"

namespace tur
{
	struct MaterialAsset : public Asset
	{
	public:
		MaterialAsset() { metadata.type = AssetType::MATERIAL; }

    public:
        std::filesystem::path vertexShaderFilepath;
        std::filesystem::path tesselationControlShader;
        std::filesystem::path tesselationEvaluationShader;
        std::filesystem::path geometryShader;
        std::filesystem::path fragmentShaderFilepath;
		
		InputAssemblyDescriptor inputAssemblyStage;
		RasterizerDescriptor rasterizerStage;
		DepthDescriptor depthDescriptor;
		BlendDescriptor blendDescriptor;
	};
}