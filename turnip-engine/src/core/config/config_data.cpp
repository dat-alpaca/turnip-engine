#include "config_data.hpp"
#include "utils/json/json_file.hpp"
#include "bridge/serialization/config.hpp"

namespace tur
{
    void initialize_config_data(const std::filesystem::path& filepath)
	{
		if (std::filesystem::exists(filepath))
			return;

		ConfigData defaultData;
		nlohmann::json data = ConfigData(); 
		write_json(filepath, defaultData);
	}
}