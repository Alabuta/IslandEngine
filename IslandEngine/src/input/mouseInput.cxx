#include "mouseInput.hxx"

namespace
{
std::bitset<16> constexpr kPRESSED_MASK{
    0x01 | 0x04 | 0x10 | 0x40 | 0x100
};

std::bitset<16> constexpr kDEPRESSED_MASK{
    0x02 | 0x08 | 0x20 | 0x80 | 0x200
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}


namespace isle
{
void MouseInput::connectHandler(std::shared_ptr<IHandler> slot)
{
    onMove_.connect(decltype(onMove_)::slot_type(
        &IHandler::onMove, slot.get(), _1, _2
    ).track_foreign(slot));

    onWheel_.connect(decltype(onWheel_)::slot_type(
        &IHandler::onWheel, slot.get(), _1, _2
    ).track_foreign(slot));

    onDown_.connect(decltype(onDown_)::slot_type(
        &IHandler::onDown, slot.get(), _1
    ).track_foreign(slot));

    onUp_.connect(decltype(onUp_)::slot_type(
        &IHandler::onUp, slot.get(), _1
    ).track_foreign(slot));
}

void MouseInput::update(input::mouse::RawData &data)
{
    std::visit(overloaded{
        [this] (input::mouse::RelativeCoords &coords)
        {
            if (coords.x != 0.f || coords.y != 0.f)
                onMove_(coords.x, coords.y);
        },
        [this] (input::mouse::Wheel wheel)
        {
            onWheel_(wheel.xoffset, wheel.yoffset);
        },
        [this] (input::mouse::Buttons &buttons)
        {
            if (buttons.value.any()) {
                auto const buttonsBitCount = kPRESSED_MASK.count();

                for (std::size_t i = 0; i < buttonsBitCount; ++i) {
                    auto const pressed = buttons.value[i];
                    auto const depressed = buttons.value[++i];

                    buttons_[i / 2] = (buttons_[i / 2] | pressed) & ~depressed;
                }

                if ((buttons.value & kPRESSED_MASK).any())
                    onDown_(buttons_);

                if ((buttons.value & kDEPRESSED_MASK).any())
                    onUp_(buttons_);
            }
        },
        [] (auto &&) { }
    }, data);
}
}
