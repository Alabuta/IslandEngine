#include "System/MouseInput.hxx"


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
                onMove_(static_cast<i32>(data.lLastX), static_cast<i32>(data.lLastY));
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
        pressed_.reset();
        depressed_.reset();

        std::bitset<16> bitset{data.usButtonFlags};

        for (std::size_t i = 0; i < bitset.size(); i += 2) {
            auto down = bitset[i];
            auto up = bitset[i + 1];

            pressed_[i / 2] = down;
            depressed_[i / 2] = up;
        }

        if (pressed_.any())
            onDown_(pressed_);

        if (depressed_.any())
            onUp_(depressed_);
    }

    switch (data.usButtonFlags) {
        case RI_MOUSE_WHEEL:
            onWheel_(static_cast<i32>(data.usButtonFlags));
            break;

        default:
            break;
    }
}
}
