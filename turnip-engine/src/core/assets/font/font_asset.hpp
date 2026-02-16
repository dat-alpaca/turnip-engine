#pragma once
#include <optional>
#include <unordered_map>
#include <freetype/freetype.h>

#include "assets/asset.hpp"
#include "assets/texture/texture_options.hpp"
#include "defines.hpp"

namespace tur
{
	constexpr inline char g_start_character = ' ';
	constexpr inline char g_end_character = 127;

	struct Character
	{
		std::vector<byte> buffer;
		glm::vec2 size;
		glm::vec2 bearing;
		i64 advance;
		u32 layer;
		char data;
	};

	struct FontAsset : public Asset
	{
	public:
		FontAsset() { metadata.type = AssetType::FONT; }

	public:
		inline std::optional<Character> get_character(char character) const 
		{
			if(characters.find(character) == characters.end())
				return std::nullopt;

			return characters.at(character);
		}
	
    public:
		FIELD_SET_INTERNALLY std::unordered_map<char, Character> characters;
		FIELD_SET_INTERNALLY FT_Face face;
		TextureOptions options;
        
		FIELD_SET_INTERNALLY u32 maxWidth = 0;
		FIELD_SET_INTERNALLY u32 maxHeight = 0;
        u32 height = 0;
		bool isSDF = false;
	};
}