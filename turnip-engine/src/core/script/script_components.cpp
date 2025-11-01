#include "script_components.hpp"
#include "script_system.hpp"

namespace tur
{
	ScriptComponent::ScriptComponent(Entity entity, const std::filesystem::path& filepath)
	{
		script.environment = ScriptManager::create_env();
		script.filepath = filepath;
		ScriptManager::initialize_entity_environment(entity, script.environment);

		script.start(ScriptManager::get_state());
	}
}