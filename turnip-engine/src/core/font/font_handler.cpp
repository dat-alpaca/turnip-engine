#include "font_handler.hpp"
#include "assets/asset_library.hpp"

namespace tur
{
	void FontHandler::initialize(NON_OWNING AssetLibrary* library)
	{
		TUR_ASSERT(library, "Invalid AssetLibrary handle passed");
		rAssetLibrary = library;

        if (FT_Init_FreeType(&mFontLibrary))
		{
            TUR_LOG_CRITICAL("Failed to initialize font library engine.");
		}
	}
	void FontHandler::shutdown()
	{
        FT_Done_FreeType(mFontLibrary);
	}
}
