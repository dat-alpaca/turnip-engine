#pragma once
#include "common.hpp"
#include "utils/string_utils.hpp"
#include <vector>

namespace tur
{
	enum class QueueOperation : u8
	{
		NONE = 0,
		GRAPHICS = 1 << 1,
		PRESENT = 1 << 2,
		COMPUTE = 1 << 3,
		TRANSFER = 1 << 4,
		SPARSE_BINDING = 1 << 5,
	};

	inline u32 operator&(QueueOperation lhs, QueueOperation rhs)
	{
		return static_cast<u32>(lhs) & static_cast<u32>(rhs);
	}

	inline u32 operator|(QueueOperation lhs, QueueOperation rhs)
	{
		return static_cast<u32>(lhs) | static_cast<u32>(rhs);
	}

	inline QueueOperation& operator|=(QueueOperation& lhs, QueueOperation rhs)
	{
		lhs = static_cast<QueueOperation>(lhs | rhs);
		return lhs;
	}

	inline QueueOperation& operator&=(QueueOperation& lhs, QueueOperation rhs)
	{
		lhs = static_cast<QueueOperation>(lhs & rhs);
		return lhs;
	}

	inline u64 count_queue_operation_matches(QueueOperation lhs, QueueOperation rhs)
	{
		u32 overlap = lhs & rhs;

		u64 count = 0;
		while (overlap)
		{
			count += overlap & 1;
			overlap >>= 1;
		}

		return count;
	}

	inline std::vector<const char*> get_queue_operation_strings(QueueOperation usage)
	{
		std::vector<const char*> usageStrings;

		if (usage & QueueOperation::NONE)
			usageStrings.push_back("none");

		if (usage & QueueOperation::GRAPHICS)
			usageStrings.push_back("graphics");

		if (usage & QueueOperation::PRESENT)
			usageStrings.push_back("present");

		if (usage & QueueOperation::COMPUTE)
			usageStrings.push_back("compute");

		if (usage & QueueOperation::TRANSFER)
			usageStrings.push_back("transfer");

		if (usage & QueueOperation::SPARSE_BINDING)
			usageStrings.push_back("sparse_binding");

		return usageStrings;
	}

	inline QueueOperation get_queue_operation_from_string(const std::string& usageString)
	{
		std::string testString = usageString;
		to_lower(testString);

		if (testString == "none")
			return QueueOperation::NONE;

		if (testString == "graphics")
			return QueueOperation::GRAPHICS;

		if (testString == "present")
			return QueueOperation::PRESENT;

		if (testString == "compute")
			return QueueOperation::COMPUTE;

		if (testString == "transfer")
			return QueueOperation::TRANSFER;

		if (testString == "sparse_binding")
			return QueueOperation::SPARSE_BINDING;

		return QueueOperation::NONE;
	}
}