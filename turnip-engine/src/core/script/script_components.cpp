#include "script_components.hpp"
#include "script_system.hpp"

namespace tur
{
	ScriptComponent::ScriptComponent(Entity entity, const std::filesystem::path& filepath)
	{
		script.environment = ScriptSystem::create_env();
		script.filepath = filepath;
		ScriptSystem::initialize_entity_environment(entity, script.environment);
		script.start(ScriptSystem::get_state());
	}
}