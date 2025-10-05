#include "pipeline.hpp"
#include "rhi/vulkan/allocators/descriptor_set.hpp"
#include "rhi/vulkan/objects/vulkan_resource_handler.hpp"
#include "rhi/vulkan/vulkan_render_interface.hpp"

// Shaders:
namespace tur::vulkan
{
	using pipeline_shaders = std::vector<vk::PipelineShaderStageCreateInfo>;

	static pipeline_shaders create_shaders(ResourceHandler& resources, const PipelineDescriptor& descriptor)
	{
		auto& shaders = resources.get_shaders();
		std::vector<vk::PipelineShaderStageCreateInfo> shaderCreateInfos = {};

		// Vertex:
		vk::PipelineShaderStageCreateInfo vertexShaderCreateInfo;
		if (descriptor.vertexShader == invalid_handle)
		{
			TUR_LOG_CRITICAL("Vertex shader not provided");
			return {};
		}

		{
			vertexShaderCreateInfo.flags = vk::PipelineShaderStageCreateFlags();
			vertexShaderCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
			vertexShaderCreateInfo.pName = "main";
			vertexShaderCreateInfo.module = shaders.get(descriptor.vertexShader);
		}
		shaderCreateInfos.push_back(vertexShaderCreateInfo);

		if (descriptor.tesselationControlShader != invalid_handle)
		{
			vk::PipelineShaderStageCreateInfo tesselationControlShaderCreateInfo;
			{
				tesselationControlShaderCreateInfo.flags = vk::PipelineShaderStageCreateFlags();
				tesselationControlShaderCreateInfo.stage = vk::ShaderStageFlagBits::eTessellationControl;
				tesselationControlShaderCreateInfo.pName = "main";
				tesselationControlShaderCreateInfo.module = shaders.get(descriptor.tesselationControlShader);
			}
			shaderCreateInfos.push_back(tesselationControlShaderCreateInfo);
		}

		if (descriptor.tesselationEvaluationShader != invalid_handle)
		{
			vk::PipelineShaderStageCreateInfo tesselationEvalShaderCreateInfo;
			{
				tesselationEvalShaderCreateInfo.flags = vk::PipelineShaderStageCreateFlags();
				tesselationEvalShaderCreateInfo.stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
				tesselationEvalShaderCreateInfo.pName = "main";
				tesselationEvalShaderCreateInfo.module = shaders.get(descriptor.tesselationEvaluationShader);
			}
			shaderCreateInfos.push_back(tesselationEvalShaderCreateInfo);
		}

		if (descriptor.geometryShader != invalid_handle)
		{
			vk::PipelineShaderStageCreateInfo geometryShaderCreateInfo;
			{
				geometryShaderCreateInfo.flags = vk::PipelineShaderStageCreateFlags();
				geometryShaderCreateInfo.stage = vk::ShaderStageFlagBits::eGeometry;
				geometryShaderCreateInfo.pName = "main";
				geometryShaderCreateInfo.module = shaders.get(descriptor.geometryShader);
			}
			shaderCreateInfos.push_back(geometryShaderCreateInfo);
		}

		// Fragment:
		vk::PipelineShaderStageCreateInfo fragmentShaderCreateInfo;

		if (descriptor.fragmentShader == invalid_handle)
		{
			TUR_LOG_CRITICAL("Fragment shader not provided");
			return {};
		}

		{
			fragmentShaderCreateInfo.flags = vk::PipelineShaderStageCreateFlags();
			fragmentShaderCreateInfo.stage = vk::ShaderStageFlagBits::eFragment;
			fragmentShaderCreateInfo.pName = "main";
			fragmentShaderCreateInfo.module = shaders.get(descriptor.fragmentShader);
		}
		shaderCreateInfos.push_back(fragmentShaderCreateInfo);

		return shaderCreateInfos;
	}
}

// Dynamic States:
namespace tur::vulkan
{
	using dynamic_states = std::vector<vk::DynamicState>;

	static vk::PipelineDynamicStateCreateInfo create_dynamic_states(const PipelineDescriptor& descriptor)
	{
		dynamic_states dynamicStates = {};

		vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		{
			dynamicStateCreateInfo.flags = vk::PipelineDynamicStateCreateFlags();

			for (const auto& descriptor : descriptor.dynamicStates)
			{
				switch (descriptor)
				{
					case DynamicState::VIEWPORT:
						dynamicStates.push_back(vk::DynamicState::eViewport);
						break;

					case DynamicState::SCISSOR:
						dynamicStates.push_back(vk::DynamicState::eScissor);
						break;
				}
			}

			dynamicStateCreateInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
			dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
		}

		return dynamicStateCreateInfo;
	}
}

// Viewport & Scissor:
namespace tur::vulkan
{
	static vk::PipelineViewportStateCreateInfo create_viewport_state(const PipelineDescriptor& descriptor)
	{
		vk::PipelineViewportStateCreateInfo pipelineViewportState = {};

		pipelineViewportState.flags = vk::PipelineViewportStateCreateFlags();
		pipelineViewportState.scissorCount = descriptor.scissors.size();
		pipelineViewportState.viewportCount = descriptor.viewports.size();

		// Viewports:
		std::vector<vk::Viewport> viewports;
		for (const auto& viewport : descriptor.viewports)
			viewports.push_back(vk::Viewport{viewport.x, viewport.y, viewport.width, viewport.height});

		pipelineViewportState.viewportCount = static_cast<u32>(viewports.size());
		pipelineViewportState.pViewports = nullptr;

		if (pipelineViewportState.viewportCount)
			pipelineViewportState.pViewports = viewports.data();

		// Scissors:
		std::vector<vk::Rect2D> scissors;
		for (const auto& scissor : descriptor.scissors)
			scissors.push_back(
				vk::Rect2D{{(float)scissor.x, (float)scissor.y}, {(float)scissor.width, (float)scissor.height}});

		pipelineViewportState.scissorCount = static_cast<u32>(scissors.size());
		pipelineViewportState.pScissors = nullptr;

		if (pipelineViewportState.scissorCount)
			pipelineViewportState.pScissors = scissors.data();

		return pipelineViewportState;
	}
}

// Input State
namespace tur::vulkan
{
	static vk::PipelineVertexInputStateCreateInfo create_input_info(const PipelineDescriptor& descriptor)
	{
		// Vertex Input:
		std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
		{
			// Bindings:
			const auto& bindings = descriptor.vertexInputStage.vertexBindings;
			bindingDescriptions.reserve(bindings.size());
			for (const auto& binding : bindings)
			{
				vk::VertexInputBindingDescription bindingDescription = {};
				bindingDescription.binding = binding.binding;
				bindingDescription.stride = binding.stride;

				bool isVertexRate = binding.inputRate == InputRate::VERTEX;
				bindingDescription.inputRate =
					isVertexRate ? vk::VertexInputRate::eVertex : vk::VertexInputRate::eInstance;

				bindingDescriptions.push_back(bindingDescription);
			}
		}

		// Attributes:
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
		{
			const auto& attributes = descriptor.vertexInputStage.attributes;
			attributeDescriptions.reserve(attributes.size());
			for (const auto& attribute : attributes)
			{
				vk::VertexInputAttributeDescription attributeDescription = {};
				attributeDescription.binding = attribute.binding;
				attributeDescription.location = attribute.location;
				attributeDescription.format = get_attribute_format(attribute.format);
				attributeDescription.offset = attribute.offset;

				attributeDescriptions.push_back(attributeDescription);
			}
		}

		// Input:
		vk::PipelineVertexInputStateCreateInfo inputCreateInfo = {};
		inputCreateInfo.flags = vk::PipelineVertexInputStateCreateFlags();

		inputCreateInfo.vertexBindingDescriptionCount = static_cast<u32>(bindingDescriptions.size());
		inputCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		inputCreateInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescriptions.size());
		inputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		return inputCreateInfo;
	}
}

// Input Assembly:
namespace tur::vulkan
{
	static vk::PipelineInputAssemblyStateCreateInfo create_input_assembly(const PipelineDescriptor& descriptor)
	{
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};

		inputAssemblyCreateInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
		inputAssemblyCreateInfo.primitiveRestartEnable = descriptor.inputAssemblyStage.primitiveRestartEnable;
		inputAssemblyCreateInfo.topology = get_primitive_topology(descriptor.inputAssemblyStage.topology);

		return inputAssemblyCreateInfo;
	}
}

// Rasterizer:
namespace tur::vulkan
{
	static vk::PipelineRasterizationStateCreateInfo create_rasterizer(const PipelineDescriptor& descriptor)
	{
		vk::PipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};

		auto& rasterizer = descriptor.rasterizerStage;

		rasterizerCreateInfo.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterizerCreateInfo.depthClampEnable = rasterizer.clampDepth;
		rasterizerCreateInfo.rasterizerDiscardEnable = rasterizer.discardFragments;

		rasterizerCreateInfo.cullMode = get_cull_mode(rasterizer.cullMode);
		rasterizerCreateInfo.frontFace = get_front_face(rasterizer.frontFace);
		rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizerCreateInfo.depthBiasClamp = 0.0f;
		rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

		rasterizerCreateInfo.depthBiasEnable = rasterizer.enableDepthBias;
		rasterizerCreateInfo.lineWidth = rasterizer.lineWidth;

		return rasterizerCreateInfo;
	}
}

// Multisampling:
namespace tur::vulkan
{
	static vk::PipelineMultisampleStateCreateInfo create_multisample()
	{
		vk::PipelineMultisampleStateCreateInfo multisampleCreateInfo = {};

		// TODO: descriptor.
		multisampleCreateInfo.flags = vk::PipelineMultisampleStateCreateFlags();
		multisampleCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

		return multisampleCreateInfo;
	}
}

// Depth & Stencil
namespace tur::vulkan
{
	static void create_depth_stencil_objects()
	{
	}
}

// Color blending:
namespace tur::vulkan
{
	static vk::PipelineColorBlendAttachmentState create_color_blend_attachment()
	{
		vk::PipelineColorBlendAttachmentState colorBlendAttachmentState = {};
		{
			// TODO: use descriptor

			vk::ColorComponentFlags colorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
														| vk::ColorComponentFlagBits::eB
														| vk::ColorComponentFlagBits::eA);

			colorBlendAttachmentState.blendEnable = false;
			colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eOne;
			colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eZero;

			colorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;

			colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
			colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;

			colorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;
			colorBlendAttachmentState.colorWriteMask = colorComponentFlags;
		}

		return colorBlendAttachmentState;
	}

	static vk::PipelineColorBlendStateCreateInfo
	create_color_blend_state(vk::PipelineColorBlendAttachmentState colorBlendAttachmentState)
	{

		vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
		{
			pipelineColorBlendStateCreateInfo.flags = vk::PipelineColorBlendStateCreateFlags();
			pipelineColorBlendStateCreateInfo.logicOpEnable = false;
			pipelineColorBlendStateCreateInfo.logicOp = vk::LogicOp::eCopy;
			pipelineColorBlendStateCreateInfo.attachmentCount = 1;
			pipelineColorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
			pipelineColorBlendStateCreateInfo.blendConstants = {{0.0f, 0.0f, 0.0f, 0.0f}};
		}

		return pipelineColorBlendStateCreateInfo;
	}
}

// Pipeline Layout:
namespace tur::vulkan
{
	static vk::PipelineLayoutCreateInfo create_pipeline_layout(vk::DescriptorSetLayout* setLayout)
	{
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		{
			pipelineLayoutCreateInfo.flags = vk::PipelineLayoutCreateFlags();
			pipelineLayoutCreateInfo.setLayoutCount = 1;
			pipelineLayoutCreateInfo.pSetLayouts = setLayout;
			pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
			pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
		}

		return pipelineLayoutCreateInfo;
	}
}

namespace tur::vulkan
{
	Pipeline allocate_graphics_pipeline(RenderInterfaceVulkan* rhi, const PipelineDescriptor& descriptor)
	{
		auto& device = rhi->get_state().logicalDevice;
		auto& swapchainFormat = rhi->get_state().swapchainFormat;

		Pipeline pipeline;
		pipeline.type = PipelineType::GRAPHICS;
		pipeline.descriptor = descriptor;

		pipeline_shaders shaders = create_shaders(rhi->get_resource_handler(), descriptor);

		vk::PipelineDynamicStateCreateInfo dynamicStates = create_dynamic_states(descriptor);
		vk::PipelineViewportStateCreateInfo viewportStates = create_viewport_state(descriptor);
		vk::PipelineVertexInputStateCreateInfo vertexInput = create_input_info(descriptor);
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = create_input_assembly(descriptor);
		vk::PipelineRasterizationStateCreateInfo rasterizer = create_rasterizer(descriptor);
		vk::PipelineMultisampleStateCreateInfo multisample = create_multisample();

		vk::PipelineColorBlendAttachmentState colorAttachment = create_color_blend_attachment();
		vk::PipelineColorBlendStateCreateInfo colorBlending = create_color_blend_state(colorAttachment);

		auto setLayout = allocate_descriptor_set_layout(rhi->get_state().logicalDevice, descriptor.descriptorSetLayout);
		vk::PipelineLayoutCreateInfo pipelineLayout = create_pipeline_layout(&setLayout);
		try
		{
			pipeline.layout = device.createPipelineLayout(pipelineLayout);
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to create pipeline layout. {}", err.what());
		}

		// Pipeline (! No renderpass since the vulkan device is using dynamic rendering):
		// TODO: rendering info
		vk::PipelineRenderingCreateInfo renderingInfo = {};
		{
			vk::Format format = swapchainFormat.format;
			renderingInfo.colorAttachmentCount = 1;
			renderingInfo.pColorAttachmentFormats = &format;
		}

		// Pipeline Info:
		vk::GraphicsPipelineCreateInfo pipelineInfo;
		{
			pipelineInfo.flags = vk::PipelineCreateFlags();

			pipelineInfo.stageCount = static_cast<u32>(shaders.size());
			pipelineInfo.pStages = shaders.data();

			pipelineInfo.pDynamicState = &dynamicStates;
			pipelineInfo.pViewportState = &viewportStates;

			pipelineInfo.pVertexInputState = &vertexInput;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisample;
			pipelineInfo.pDepthStencilState = nullptr;
			pipelineInfo.pColorBlendState = &colorBlending;

			pipelineInfo.layout = pipeline.layout;
			pipelineInfo.pNext = &renderingInfo;
		}

		try
		{
			auto result = device.createGraphicsPipeline(nullptr, pipelineInfo);
			switch (result.result)
			{
				case vk::Result::eSuccess:
					break;

				case vk::Result::ePipelineCompileRequiredEXT:
					TUR_LOG_CRITICAL(
						"VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT_EXT on PipelineCreateInfo");
					break;

				default:
					TUR_LOG_CRITICAL("Pipeline creation gone wild");
			}

			pipeline.pipeline = result.value;
		}
		catch (vk::SystemError& err)
		{
			TUR_LOG_CRITICAL("Failed to create graphics pipeline. {}", err.what());
		}

		return pipeline;
	}
}