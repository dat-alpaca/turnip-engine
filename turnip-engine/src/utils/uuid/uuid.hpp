#pragma once
#include <numeric>

#include "common.hpp"
#include "utils/random/random.hpp"

namespace tur
{
	using uuid = u64;
	constexpr inline uuid invalid_uuid = std::numeric_limits<u64>::max();

	inline uuid generate_uuid()
	{
		return generate_random<u64>(std::numeric_limits<u64>::min(), std::numeric_limits<u64>::max() - 1);
	}

	inline bool is_uuid_valid(uuid uuid)
	{
		return uuid != invalid_uuid;
	}
}