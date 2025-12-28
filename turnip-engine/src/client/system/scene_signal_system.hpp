#pragma once
#include "assert/assert.hpp"
#include "assets/asset_library.hpp"
#include "audio/audio_components.hpp"
#include "defines.hpp"

#include "event/asset_events/scene_deserialization.hpp"
#include "graphics/components.hpp"
#include "memory/memory.hpp"
#include "scene/scene.hpp"

namespace tur
{
	class SceneSignalSystem
	{
	public:
		void initialize(NON_OWNING AssetLibrary* library)
		{
            rLibrary = library;
		}

		void on_event(Event& event)
		{
			Subscriber subscriber(event);
			subscriber.subscribe<SceneDeserializedEvent>(
				[&](const SceneDeserializedEvent& deserializedEvent) -> bool
				{
                    handle_textures(deserializedEvent.scene);
                    handle_audio(deserializedEvent.scene);
                    return false;
				}
			);
		}

    private:
        void handle_textures(NON_OWNING Scene* scene)
        {
            TUR_ASS(rLibrary);
            TUR_ASS(scene);

            auto viewSprite = scene->get_registry().view<Sprite2DComponent>();
            for (auto [e, sprite] : viewSprite.each())
            {
                if(sprite.assetHandle != invalid_handle)
                    continue;

                sprite.assetHandle = rLibrary->get_asset_handle(sprite.filepath);
            }

            auto viewTilemap = scene->get_registry().view<Tilemap2DComponent>();
            for (auto [e, tilemap] : viewTilemap.each())
            {
                if(tilemap.assetHandle != invalid_handle)
                    continue;

                tilemap.assetHandle = rLibrary->get_asset_handle(tilemap.filepath);
            }
        }

        void handle_audio(NON_OWNING Scene* scene)
        {
            TUR_ASS(rLibrary);
            TUR_ASS(scene);

            auto view = scene->get_registry().view<AudioSourceComponent>();
            for (auto [e, audio] : view.each())
            {
                if(audio.assetHandle != invalid_handle)
                    continue;

                audio.assetHandle = rLibrary->get_asset_handle(audio.filepath);
            }
        }

	private:
		NON_OWNING AssetLibrary* rLibrary = nullptr;
	};
}