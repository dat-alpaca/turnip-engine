#pragma once
#include "assets/font/font_asset.hpp"
#include <optional>

namespace tur
{
	inline std::optional<FontAsset> load_font_task(const std::filesystem::path& filepath, FT_Library fonts, const AssetMetadata& metadata, const TextureOptions& options, u32 height, bool loadSDF)
	{
        FontAsset fontAsset;

		if (!std::filesystem::exists(filepath))
		{
			TUR_LOG_ERROR("Failed to load texture: '{}'. File does not exist.", filepath.string());
			return std::nullopt;
		}

        if (FT_New_Face(fonts, filepath.string().c_str(), 0, &fontAsset.face))
		{
			TUR_LOG_ERROR("Failed to load font face: {}", filepath.string());
			return std::nullopt;
		}

		FT_Set_Pixel_Sizes(fontAsset.face, 0, height);

		for(unsigned char character = g_start_character; character <= g_end_character; ++character)
		{
			u32 index = static_cast<u64>(character - g_start_character);

			auto& face = fontAsset.face;
			auto& glyph = face->glyph;

			FT_Int32 loadFlags = !loadSDF ? FT_LOAD_RENDER : FT_LOAD_DEFAULT;
			if (FT_Load_Char(face, character, loadFlags))
			{
				TUR_LOG_ERROR("Failed to load glyph: {}", character);
				continue;
			}

			if(loadSDF)
				FT_Render_Glyph(glyph, FT_RENDER_MODE_SDF);

			// Buffer copying:
			u64 size = (glyph->bitmap.rows) * std::abs(glyph->bitmap.pitch);
			std::vector<byte> buffer(
				reinterpret_cast<byte*>(glyph->bitmap.buffer),
				reinterpret_cast<byte*>(glyph->bitmap.buffer) + size
			);

			fontAsset.characters[character] = {
				.buffer = buffer,
				.size = { glyph->bitmap.width, glyph->bitmap.rows },
				.bearing = { glyph->bitmap_left, glyph->bitmap_top },
				.advance = glyph->advance.x,
				.layer = index,
				.data = static_cast<char>(character)
			};

			fontAsset.maxWidth = std::max((u64)fontAsset.maxWidth, (u64)glyph->bitmap.width);
			fontAsset.maxHeight = std::max((u64)fontAsset.maxHeight, (u64)glyph->bitmap.rows);
		}

		return fontAsset;
	}
}