#include "System/MouseInput.hxx"

namespace
{
std::bitset<16> constexpr kPRESSED_MASK{
    RI_MOUSE_BUTTON_1_DOWN |
    RI_MOUSE_BUTTON_2_DOWN |
    RI_MOUSE_BUTTON_3_DOWN |
    RI_MOUSE_BUTTON_4_DOWN |
    RI_MOUSE_BUTTON_5_DOWN
};

std::bitset<16> constexpr kDEPRESSED_MASK{
    RI_MOUSE_BUTTON_1_UP |
    RI_MOUSE_BUTTON_2_UP |
    RI_MOUSE_BUTTON_3_UP |
    RI_MOUSE_BUTTON_4_UP |
    RI_MOUSE_BUTTON_5_UP
};
}

namespace isle
{
void MouseInput::connect(std::weak_ptr<IHandler> slot)
{
    onMove_.connect(decltype(onMove_)::slot_type([&] (auto x, auto y) {
        if (auto ptr = slot.lock(); ptr)
            ptr->onMove(x, y);
    }).track_foreign(slot));

    onWheel_.connect(decltype(onWheel_)::slot_type([&] (auto wheel) {
        if (auto ptr = slot.lock(); ptr)
            ptr->onWheel(wheel);
    }).track_foreign(slot));

    onDown_.connect(decltype(onDown_)::slot_type([&] (auto button) {
        if (auto ptr = slot.lock(); ptr)
            ptr->onDown(button);
    }).track_foreign(slot));

    onUp_.connect(decltype(onUp_)::slot_type([&] (auto button) {
        if (auto ptr = slot.lock(); ptr)
            ptr->onUp(button);
    }).track_foreign(slot));
}

void MouseInput::update(RAWMOUSE &&data)
{
    switch (data.usFlags) {
        case MOUSE_MOVE_RELATIVE:
            if (data.lLastX || data.lLastY)
                onMove_(data.lLastX, data.lLastY);
            break;

        case MOUSE_MOVE_ABSOLUTE:
            log::Debug() << "MOUSE_MOVE_ABSOLUTE";
            break;

        case MOUSE_VIRTUAL_DESKTOP:             // For a multiple monitor system.
            log::Warning() << "MOUSE_VIRTUAL_DESKTOP";
            break;

        case MOUSE_ATTRIBUTES_CHANGED:          // Needs to query the attributes.
            log::Warning() << "MOUSE_ATTRIBUTES_CHANGED";
            break;
    }    
    
    if (data.usButtonFlags) {
        std::bitset<16> bitset{data.usButtonFlags};

        for (std::size_t i = 0; i < bitset.size(); ++i) {
            auto const pressed = bitset[i];
            auto const depressed = bitset[++i];

            buttons_[i / 2] = (buttons_[i / 2] | pressed) & ~depressed;
        }

        if ((bitset & kPRESSED_MASK).any())
            onDown_(buttons_);

        if ((bitset & kDEPRESSED_MASK).any())
            onUp_(buttons_);
    }

    switch (data.usButtonFlags) {
        case RI_MOUSE_WHEEL:
            onWheel_(static_cast<i16>(data.usButtonData));
            break;

        default:
            break;
    }
}
}
