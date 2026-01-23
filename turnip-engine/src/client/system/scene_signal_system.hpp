#pragma once
#include "assert/assert.hpp"
#include "assets/asset_library.hpp"
#include "assets/cubemap/cubemap_asset.hpp"
#include "audio/audio_components.hpp"
#include "defines.hpp"

#include "event/asset_events/scene_deserialization.hpp"
#include "graphics/components.hpp"
#include "memory/memory.hpp"
#include "scene/scene.hpp"
#include "script/script_components.hpp"
#include <sol/types.hpp>

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
                    handle_textures(deserializedEvent.scene, deserializedEvent.project);
                    handle_audio(deserializedEvent.scene, deserializedEvent.project);
                    handle_script(deserializedEvent.scene, deserializedEvent.project);
                    handle_meshes(deserializedEvent.scene, deserializedEvent.project);
                    return false;
				}
			);
		}

    private:
        void handle_textures(NON_OWNING Scene* scene, const Project& project)
        {
            TUR_ASS(rLibrary);
            TUR_ASS(scene);

            auto viewSprite = scene->get_registry().view<Sprite2DComponent>();
            for (auto [e, sprite] : viewSprite.each())
            {
                if(sprite.assetHandle != invalid_handle)
                    continue;

                sprite.assetHandle = rLibrary->get_asset_handle(sprite.uuid);
            }

            auto viewTilemap = scene->get_registry().view<Tilemap2DComponent>();
            for (auto [e, tilemap] : viewTilemap.each())
            {
                if(tilemap.assetHandle != invalid_handle)
                    continue;

                tilemap.assetHandle = rLibrary->get_asset_handle(tilemap.uuid);
            }

            auto viewCubemap = scene->get_registry().view<CubemapComponent>();
            for (auto [e, cubemap] : viewCubemap.each())
            {
                if(cubemap.assetHandle != invalid_handle)
                    continue;

                cubemap.assetHandle = rLibrary->get_asset_handle(cubemap.uuid);
            }
        }

        void handle_audio(NON_OWNING Scene* scene, const Project& project)
        {
            TUR_ASS(rLibrary);
            TUR_ASS(scene);

            auto view = scene->get_registry().view<AudioSourceComponent>();
            for (auto [e, audio] : view.each())
            {
                if(audio.assetHandle != invalid_handle)
                    continue;

                audio.assetHandle = rLibrary->get_asset_handle(audio.uuid);
            }
        }

        void handle_script(NON_OWNING Scene* scene, const Project& project)
        {
            TUR_ASS(rLibrary);
            TUR_ASS(scene);

            auto view = scene->get_registry().view<ScriptComponent>();
            for (auto [e, script] : view.each())
            {
                if(script.loaded)
                    continue;

                script.filepath = project.get_project_filepath(script.filepath);
            }
        }

        void handle_meshes(NON_OWNING Scene* scene, const Project& project)
        {
            TUR_ASS(rLibrary);
            TUR_ASS(scene);

            auto view = scene->get_registry().view<MeshComponent>();
            for (auto [e, mesh] : view.each())
            {
                if(mesh.assetHandle != invalid_handle)
                    continue;

                mesh.assetHandle = rLibrary->get_asset_handle(mesh.uuid);
            }
        }

	private:
		NON_OWNING AssetLibrary* rLibrary = nullptr;
	};
}