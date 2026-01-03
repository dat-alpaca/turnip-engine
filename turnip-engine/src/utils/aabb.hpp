#pragma once
#include <glm/glm.hpp>
#include "utils/extent.hpp"

namespace tur
{
    inline bool is_point_inside(float left, float right, float bottom, float top, const glm::vec2& point)
    {
        return point.x >= left && point.x <= right && point.y >= bottom && point.y <= top;
    }

    inline bool is_colliding_aabb(const Rect& lhs, const Rect& rhs)
    {
        return (
            rhs.x + rhs.width  > lhs.x  &&
            rhs.y + rhs.height > lhs.y  &&
            lhs.x + lhs.width  > rhs.x  &&
            lhs.y + lhs.height > rhs.y
        );
    }
}