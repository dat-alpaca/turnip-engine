#include "audio/audio_components.hpp"
#include "audio/audio_handler.hpp"
#include "scene/entity.hpp"
#include "script_system.hpp"

namespace tur
{
	void ScriptManager::initialize_audio_environment(Entity entity, sol::environment environment)
	{
		environment["AudioSourceComponent"]["play"] = [&](AudioSourceComponent* component)
		{
			asset_handle assetHandle = component->assetHandle;
			srAudioHandler->play(assetHandle);
		};

		environment["AudioSourceComponent"]["set_volume"] = [&](AudioSourceComponent* component, float volume)
		{
			asset_handle assetHandle = component->assetHandle;
			srAudioHandler->set_volume(assetHandle, volume);
		};
	}
}