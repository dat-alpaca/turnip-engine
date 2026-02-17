#pragma once
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <limits>

#define TUR_PROJECT_NAME "Turnip"
#define TUR_ENGINE_NAME "TurnipEngine"

#define TUR_ENGINE_VERSION_MAJOR 1
#define TUR_ENGINE_VERSION_MINOR 1

#if defined(_DEBUG) || !defined(NDEBUG)
	#define TUR_DEBUG
#else
	#define TUR_RELEASE
#endif

#if defined(WIN32) || defined(_WIN32)
	#define TUR_PLATFORM_WIN32
	#define TUR_BREAKPOINT() __debugbreak()
#elif defined(__linux__)
	#define TUR_PLATFORM_LINUX
	#define TUR_BREAKPOINT() __builtin_trap()
#elif defined(__APPLE__)
	#define TUR_PLATFORM_APPLE
	#define TUR_BREAKPOINT() __builtin_trap()
#elif defined(__ANDROID__)
	#define TUR_PLATFORM_ANDROID
	#define TUR_BREAKPOINT() __builtin_trap()
#else
	#error Unsupported platform
#endif

#define FIELD_SET_INTERNALLY

#define STRINGIFY(string) #string
#define REMOVE_PREFIX(string, amount) std::string_view(STRINGIFY(string)).substr(amount)

namespace tur
{
	using u8 = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using f32 = float;
	using f64 = double;

	using byte = u8;

	using handle_type = u32;
	constexpr handle_type invalid_handle = std::numeric_limits<handle_type>::max();

	constexpr glm::uvec2 invalid_size =
		glm::uvec2(std::numeric_limits<handle_type>::max(), std::numeric_limits<handle_type>::max());
}