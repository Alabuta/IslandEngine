#pragma once

#include <bitset>
#include <variant>


namespace input
{
    namespace mouse
    {
        struct Buttons final {
            std::bitset<16> value;
        };

        struct RelativeCoords final {
            float x, y;
        };

        struct Wheel final {
            float xoffset, yoffset;
        };

        using RawData = std::variant<
            Buttons, RelativeCoords, Wheel
        >;
    }

    namespace keyboard
    {
        using RawData = bool;
    }

    using RawData = std::variant<mouse::RawData, keyboard::RawData>;
}
