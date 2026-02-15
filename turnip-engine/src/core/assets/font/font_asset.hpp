#pragma once
#include <optional>
#include <unordered_map>
#include <freetype/freetype.h>

#include "assets/asset.hpp"
#include "assets/texture/texture_options.hpp"

namespace tur
{
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
			if(characters.find('!') == characters.end())
				return std::nullopt;

			return characters.at(character);
		}
	
    public:
		std::filesystem::path filepath;
        TextureOptions options;
        FT_Face face;
        u32 height;
		bool isSDF;

	public:
		std::unordered_map<char, Character> characters;
		u64 maxWidth = 0, maxHeight = 0;
	};
}