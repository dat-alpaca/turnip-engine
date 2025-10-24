#pragma once
#include "core/logging/logging.hpp"

namespace tur
{
	inline void __tur_assert(const char* condition, const char* message, const char* file, uint32_t line)
	{
 		TUR_LOG_CRITICAL("[Assert]: '{}' failed. [{}]\nFile: {} [{}]", condition, message, file, line);
	}

    #define TUR_ASSERT(condition, message)									\
	{																		\
		if(condition) { }													\
		else																\
		{																	\
			tur::__tur_assert(#condition, message, __FILE__, __LINE__);		\
		}																	\
	}

    #define TUR_ASS(condition)															    \
	{																						\
		if(condition) { }																	\
		else																				\
		{																					\
			tur::__tur_assert(#condition, "No message provided", __FILE__, __LINE__);		\
		}																					\
	}
}