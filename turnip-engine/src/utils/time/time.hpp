#pragma once
#include "common.hpp"

namespace tur
{
    class Time
    {
    public:
        Time()
        {
            mPreviousTimestamp = 0.0f;
            mCurrentTimestamp = 0.0f;
            mDeltaTime = 0.0f;
            mAccumulatedTime = 0.0f;
        }

    public:      
        void cycle(f64 currentTime)
        {
            mCurrentTimestamp = currentTime;
            mDeltaTime = mCurrentTimestamp - mPreviousTimestamp;
            mAccumulatedTime += mDeltaTime;
            mPreviousTimestamp = mCurrentTimestamp;
        }
   
    public:
        inline f64 get_delta_time() const
        {
            return mDeltaTime;
        }
        inline f64 get_fps() const 
        {
            return 1.0f / mDeltaTime;
        }
        inline u64 get_accumulated_time() const
        {
            return mAccumulatedTime;
        }
        inline u32 get_accumulated_milliseconds() const
        {
            return static_cast<u32>(mAccumulatedTime * 1000.f);
        }

    private:
        f64 mPreviousTimestamp = 0.0f;
        f64 mCurrentTimestamp = 0.0f;
        f64 mDeltaTime = 0.0f;
        f64 mAccumulatedTime = 0.0f;
    };
}