#pragma once
#include "common.hpp"
#include <vector>

namespace tur
{
	template <typename T>
	class static_ring_buffer
	{
		using size_type = u64;

	public:
		static_ring_buffer(size_type size)
		{
			TUR_ASSERT(size > 0, "Size must be positive and non-zero");
			resize(size);
		}

		static_ring_buffer() = default;

	public:
		constexpr void set(const T& data, u64 index) { mData[index] = data; }
		constexpr void set(T&& data, u64 index) { mData[index] = std::move(data); }

		constexpr inline void advance() noexcept { mCurrentIndex = (mCurrentIndex + 1) % mSize; }

		inline void clear() noexcept { mData.clear(); }
		inline void resize(size_type size) noexcept
		{
			TUR_ASSERT(size > 0, "Size must be positive and non-zero");
			mData.resize(size);
			mSize = mData.size();
		}

	public:
		constexpr auto begin() noexcept { return mData.begin(); }
		constexpr auto end() noexcept { return mData.end(); }
		constexpr auto begin() const noexcept { return mData.begin(); }
		constexpr auto end() const noexcept { return mData.end(); }

	public:
		constexpr inline T& get_current() { return mData[mCurrentIndex]; }
		constexpr inline const T& get_current() const { return mData[mCurrentIndex]; }

		constexpr inline std::vector<T>& get_data() { return mData; }

	public:
		constexpr inline size_type current_index() const { return mCurrentIndex; }
		constexpr inline size_type capacity() const { return mSize; }

	private:
		std::vector<T> mData;
		size_type mCurrentIndex = 0;
		size_type mSize;
	};
}