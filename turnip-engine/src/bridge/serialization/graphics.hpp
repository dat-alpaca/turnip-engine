#pragma once
#include <json.hpp>
#include "graphics/objects/pipeline.hpp"

namespace tur
{   
    NLOHMANN_JSON_SERIALIZE_ENUM(PrimitiveTopology,
    {
        { PrimitiveTopology::POINTS, "POINTS" },
        { PrimitiveTopology::LINES, "LINES" },
        { PrimitiveTopology::LINE_STRIPS, "LINE_STRIPS" },
        { PrimitiveTopology::TRIANGLES, "TRIANGLES" },
        { PrimitiveTopology::TRIANGLE_STRIPS, "TRIANGLE_STRIPS" },
        { PrimitiveTopology::TRIANGLE_FANS, "TRIANGLE_FANS" },
        { PrimitiveTopology::LINES_WITH_ADJACENCY, "LINES_WITH_ADJACENCY" },
        { PrimitiveTopology::LINE_STRIPS_WITH_ADJACENCY, "LINE_STRIPS_WITH_ADJACENCY" },
        { PrimitiveTopology::TRIANGLES_WITH_ADJACENCY, "TRIANGLES_WITH_ADJACENCY" },
        { PrimitiveTopology::TRIANGLE_STRIPS_WITH_ADJACENCY, "TRIANGLE_STRIPS_WITH_ADJACENCY" },
        { PrimitiveTopology::PATCHES, "PATCHES" },
    });

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputAssemblyDescriptor, topology, primitiveRestartEnable);
}

namespace tur
{   
    NLOHMANN_JSON_SERIALIZE_ENUM(FrontFace,
    {
        { FrontFace::COUNTER_CLOCKWISE, "COUNTER_CLOCKWISE" },
        { FrontFace::CLOCKWISE, "CLOCKWISE" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(CullMode,
    {
        { CullMode::NONE, "NONE" },
        { CullMode::FRONT, "FRONT" },
        { CullMode::BACK, "BACK" },
        { CullMode::FRONT_AND_BACK, "FRONT_AND_BACK" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(PolygonMode,
    {
        { PolygonMode::FILL, "FILL" },
        { PolygonMode::LINE, "LINE" },
        { PolygonMode::POINT, "POINT" },
        { PolygonMode::FILL_RECTANGLE, "FILL_RECTANGLE" },
    });

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        RasterizerDescriptor, 
        frontFace, cullMode, polygonMode,
        lineWidth, discardFragments, enableDepthBias,
        clampDepth
    );
}

namespace tur
{   
    NLOHMANN_JSON_SERIALIZE_ENUM(DepthCompareOp,
    {
        { DepthCompareOp::NEVER, "NEVER" },
        { DepthCompareOp::ALWAYS, "ALWAYS" },
        { DepthCompareOp::EQUAL, "EQUAL" },
        { DepthCompareOp::NOT_EQUAL, "NOT_EQUAL" },
        { DepthCompareOp::LESS, "LESS" },
        { DepthCompareOp::LESS_OR_EQUAL,"LESS_OR_EQUAL" },
        { DepthCompareOp::GREATER, "GREATER" },
        { DepthCompareOp::GREATER_OR_EQUAL, "GREATER_OR_EQUAL" }
    });  

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        DepthDescriptor, 
        depthTestEnable, depthWriteEnable, compareOp
    );
}

namespace tur
{   
    NLOHMANN_JSON_SERIALIZE_ENUM(BlendFactor,
	{
		{ BlendFactor::ZERO, "ZERO" },
		{ BlendFactor::ONE, "ONE" },
		{ BlendFactor::SRC_COLOR, "SRC_COLOR" },
		{ BlendFactor::ONE_MINUS_SRC_COLOR, "ONE_MINUS_SRC_COLOR" },
		{ BlendFactor::DST_COLOR, "DST_COLOR" },
		{ BlendFactor::ONE_MINUS_DST_COLOR, "ONE_MINUS_DST_COLOR" },
		{ BlendFactor::SRC_ALPHA, "SRC_ALPHA" },
		{ BlendFactor::ONE_MINUS_SRC_ALPHA, "ONE_MINUS_SRC_ALPHA" },
		{ BlendFactor::DST_ALPHA, "DST_ALPHA" },
		{ BlendFactor::ONE_MINUS_DST_ALPHA, "ONE_MINUS_DST_ALPHA" },
		{ BlendFactor::CONSTANT_COLOR, "CONSTANT_COLOR" },
		{ BlendFactor::ONE_MINUS_CONSTANT_COLOR, "ONE_MINUS_CONSTANT_COLOR" },
		{ BlendFactor::CONSTANT_ALPHA, "CONSTANT_ALPHA" },
		{ BlendFactor::ONE_MINUS_CONSTANT_ALPHA, "ONE_MINUS_CONSTANT_ALPHA" },
		{ BlendFactor::SRC_ALPHA_SATURATE, "SRC_ALPHA_SATURATE" },
		{ BlendFactor::SRC1_COLOR, "SRC1_COLOR" },
		{ BlendFactor::ONE_MINUS_SRC1_COLOR, "ONE_MINUS_SRC1_COLOR" },
		{ BlendFactor::SRC1_ALPHA, "SRC1_ALPHA" },
		{ BlendFactor::ONE_MINUS_SRC1_ALPHA, "ONE_MINUS_SRC1_ALPHA" },
	});

	NLOHMANN_JSON_SERIALIZE_ENUM(BlendFunc,
	{
		{ BlendFunc::ADD, "ADD" }, 
    	{ BlendFunc::SUBTRACT, "SUBTRACT" }, 
		{ BlendFunc::REVERSE_SUBTRACT, "REVERSE_SUBTRACT" }, 
		{ BlendFunc::MIN, "MIN" }, 
		{ BlendFunc::MAX, "MAX" },   
 	});

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        BlendDescriptor, 
        enable,
        srcColorBlendFactor, dstColorBlendFactor, colorBlendFunc,
        srcAlphaBlendFactor, dstAlphaBlendFactor, alphaBlendFunc
    );
}
namespace tur
{
    NLOHMANN_JSON_SERIALIZE_ENUM(TextureFormat,
	{
		{ TextureFormat::R8_UNORM, "R8_UNORM" }, 
    	{ TextureFormat::RG8_UNORM, "RG8_UNORM" }, 
		{ TextureFormat::RGB8_UNORM, "RGB8_UNORM" }, 
		{ TextureFormat::RGBA8_UNORM, "RGBA8_UNORM" }, 
		{ TextureFormat::RGBA16_SFLOAT, "RGBA16_SFLOAT" },   
		{ TextureFormat::DEPTH_16_UNORM, "DEPTH_16_UNORM" },   
		{ TextureFormat::DEPTH_STENCIL24_S8U_INT, "DEPTH_STENCIL24_S8U_INT" },   
		{ TextureFormat::B8G8R8A8_UNORM, "B8G8R8A8_UNORM" },   
 	});
}