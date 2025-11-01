#pragma once
#include "common.hpp"
#include <deque>
#include <vector>

namespace tur
{
	template <typename Type>
	class free_list
	{
		using iterator = Type*;
		using const_iterator = const Type*;

	public:
		handle_type add(const Type& data)
		{
			if (mFreeList.empty())
			{
				mData.push_back(data);
				return static_cast<handle_type>(mData.size() - 1);
			}

			handle_type emptyHandle = mFreeList.back();
			mFreeList.pop_back();

			mData[emptyHandle] = data;
			return emptyHandle;
		}

		Type remove(handle_type handle)
		{
			mFreeList.push_back(handle);
			return mData[handle];
		}

		Type& get(handle_type handle)
		{
#ifdef TUR_DEBUG
			if (!is_valid_handle(handle))
				TUR_LOG_CRITICAL("Invalid handle: {}", handle);
#endif
			return mData[handle];
		}

		bool is_marked_for_deletion(handle_type handle) const
		{
			return std::find(mFreeList.begin(), mFreeList.end(), handle) == mFreeList.end();
		}

		void clear()
		{
			mData.clear();
			mFreeList.clear();
		}

		size_t size() const
		{
			return mData.size();
		}

	public:
		const_iterator cbegin() const
		{
			return mData.data();
		}
		const_iterator cend() const
		{
			return mData.data() + mData.size();
		}
		const_iterator begin() const
		{
			return cbegin();
		}
		const_iterator end() const
		{
			return cend();
		}

		iterator begin()
		{
			return mData.data();
		}
		iterator end()
		{
			return mData.data() + mData.size();
		}

	public:
		std::vector<handle_type> available_indices() const
		{
			std::vector<handle_type> available;
			available.reserve(mData.size() - mFreeList.size());

			for (u64 i = 0; i < mData.size(); ++i)
			{
				// TODO: occupation list : marks as true if element is not in the free list
				if (std::find(mFreeList.begin(), mFreeList.end(), i) != mFreeList.end())
					continue;

				available.push_back(i);
			}

			return available;
		}

		std::vector<std::reference_wrapper<Type>> available() const
		{
			std::vector<std::reference_wrapper<Type>> available;
			available.reserve(mData.size() - mFreeList.size());

			for (u64 i = 0; i < mData.size(); ++i)
			{
				// TODO: occupation list : marks as true if element is not in the free list
				if (std::find(mFreeList.begin(), mFreeList.end(), i) != mFreeList.end())
					continue;

				available.push_back(mData[i]);
			}

			return available;
		}

	private:
		inline bool is_valid_handle(handle_type handle) const
		{
			for (const auto& emptyHandle : mFreeList)
			{
				if (handle == emptyHandle)
					return false;
			}

			return true;
		}

	private:
		std::vector<Type> mData;
		std::deque<handle_type> mFreeList;
	};
}