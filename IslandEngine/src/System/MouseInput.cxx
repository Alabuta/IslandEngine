#include "System/MouseInput.hxx"

namespace
{
template<class T, typename std::enable_if_t<std::is_enum_v<std::decay_t<T>>>...>
struct bitflag {
    constexpr bitflag<T> operator&= (T const &flag) const noexcept
    {
        using U = std::underlying_type_t<std::decay_t<T>>;

        *this = static_cast<U>(*this) | static_cast<U>(flag);

        return *this;
    }
};

namespace bittest
{
template<class T, typename std::enable_if_t<std::is_enum_v<T>>...>
T constexpr operator| (T lhs, T rhs)
{
    using U = typename std::underlying_type_t<T>;

    return static_cast<U>(lhs) | static_cast<U>(rhs);
}
}
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
            /*relat_x_ = -static_cast<i16>(_data->lLastX) * sense_x_;
            relat_y_ = -static_cast<i16>(_data->lLastY) * sense_y_;*/
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

    buttonsDown_.reset();
    buttonsUp_.reset();

    switch (data.usButtonFlags) {
        case RI_MOUSE_BUTTON_1_DOWN:
        case RI_MOUSE_BUTTON_2_DOWN:
        case RI_MOUSE_BUTTON_3_DOWN:
            buttonsDown_.set(data.usButtonFlags - 1);
            break;

        case RI_MOUSE_BUTTON_1_UP:
        case RI_MOUSE_BUTTON_2_UP:
        case RI_MOUSE_BUTTON_3_UP:
            buttonsUp_.set(data.usButtonFlags / 2 - 1);
            break;

        /*case RI_MOUSE_WHEEL:
            //wheel_ = _data->usButtonFlags;
            state_ ^= nSTATE::WHEEL;
            break;*/

        default:
            break;
    }

    if (buttonsDown_.any())
        onDown_(8);

    if (buttonsUp_.any())
        onUp_(2);

    /*onMove_(4, 8);
    onWheel_(-1);*/
}
}
