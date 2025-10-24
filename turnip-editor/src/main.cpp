#include <turnip_engine.hpp>

#include "views/project/project_view.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
using namespace tur;

int main()
{
	TurnipEngine engine;
	initialize_turnip_engine(engine, "res/engine_config.json");

	engine_add_view(engine, tur::make_unique<ProjectView>());
	turnip_engine_run(engine);

	return 0;
}
