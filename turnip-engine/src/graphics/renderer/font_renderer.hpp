#pragma once
#include "defines.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/camera.hpp"

#include "rhi/rhi.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tur
{
	struct CharacterEntry
	{
		glm::vec3 position;
		float __pad0;
		
		glm::vec2 dimensions;
		glm::vec2 uvSize;
		
		u32 layer;
		u32 __pad[3];
	};

	class FontRenderer : public Renderer
	{
	public:
		struct WorldUBO
		{
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 projection;
		};

	public:
		void initialize(NON_OWNING RenderInterface* rhi);
		void set_camera(NON_OWNING Camera* camera);
		void render();

    public:
		void set_model(const glm::mat4& model);
		void set_font_texture(texture_handle textureHandle);
        void set_character_data(const std::vector<CharacterEntry>& character);

	private:
		void initialize_resources();
		void initialize_buffers();
		void initialize_descriptors();
		void initialize_pipeline();

	public:
		bool is_empty() const { return mCharacterEntries.empty(); }

	private:
		NON_OWNING RenderInterface* rRHI = nullptr;
		NON_OWNING Camera* rCamera = nullptr;
	
	private:
		std::vector<CharacterEntry> mCharacterEntries;
		glm::mat4 mModel;

		descriptor_set_layout_handle setLayout;
		descriptor_set_handle mainSet;

		pipeline_handle pipeline;
		buffer_handle worldUBO, ssbo;
		texture_handle fontAtlasHandle = invalid_handle;

	private:
		static inline constexpr u32 MaxCharacterAmountSSBO = 1024;
	};
}