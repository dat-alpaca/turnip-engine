#pragma once
#include <ft2build.h>
#include <freetype/freetype.h>

#include "assets/asset.hpp"
#include "common.hpp"

namespace tur
{
	class AssetLibrary;

	class FontHandler
	{
	public:
		void initialize(NON_OWNING AssetLibrary* library);
		void shutdown();

	public:
		FT_Library& get_font_library() { return mFontLibrary; }

	public:
		NON_OWNING AssetLibrary* rAssetLibrary = nullptr;
        FT_Library mFontLibrary;
	};
}