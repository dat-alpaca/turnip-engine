#pragma once
#include <glm/glm.hpp>
#include <sstream>
#include <string>

#include "common.hpp"

namespace tur
{
    struct Color
    {
    public:
        explicit Color(const glm::vec4& color)
            : colorData(color)
        {
        }

        Color(u32 r, u32 g, u32 b, u32 a = 255)
            : colorData(
                static_cast<f32>(r) / 255.f,
                static_cast<f32>(g) / 255.f,
                static_cast<f32>(b) / 255.f,
                static_cast<f32>(a) / 255.f) {}

        Color(const std::string& value)
        {
            std::string temp(value);
            if (temp.at(0) == '#')
                temp = temp.erase(0, 1);

            u32 r = 0, g = 0, b = 0, a = 255;
            if (temp.size() >= 2)
                std::istringstream(temp.substr(0, 2)) >> std::hex >> r;

            if (temp.size() >= 4)
                std::istringstream(temp.substr(2, 2)) >> std::hex >> g;

            if (temp.size() >= 6)
                std::istringstream(temp.substr(4, 2)) >> std::hex >> b;

            if (temp.size() >= 8)
                std::istringstream(temp.substr(6, 2)) >> std::hex >> a;

            colorData = {static_cast<f32>(r) / 255.f,
                         static_cast<f32>(g) / 255.f,
                         static_cast<f32>(b) / 255.f,
                         static_cast<f32>(a) / 255.f};
        }

        Color() = default;

    public:
        union
        {
            glm::vec4 colorData = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            struct
            {
                f32 r, g, b, a;
            };
        };
    };

    namespace color
    {
        inline Color Black = Color(000, 000, 000, 255);
        inline Color White = Color(255, 255, 255, 255);

        inline Color Red = Color(255, 000, 000, 255);
        inline Color Green = Color(000, 255, 000, 255);
        inline Color Blue = Color(000, 000, 255, 255);
    }
}