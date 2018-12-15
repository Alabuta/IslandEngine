#pragma once

#include <memory>
#include <bitset>

#include <boost/signals2.hpp>



using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;


namespace isle
{
struct raw_mouse_t { };

class MouseInput final {
public:

    class IHandler {
    public:

        using buttons_t = std::bitset<8>;

        virtual ~IHandler() = default;

        virtual void onMove(i64 x, i64 y) = 0;
        virtual void onWheel(i16 delta) = 0;
        virtual void onDown(buttons_t buttons) = 0;
        virtual void onUp(buttons_t buttons) = 0;
    };

    void connect(std::shared_ptr<IHandler> slot);

    void update(raw_mouse_t &&data);

private:

    IHandler::buttons_t buttons_;
/*
    boost::signals2::signal<void(i32, i32)> onMove_;
    boost::signals2::signal<void(i32)> onWheel_;
    boost::signals2::signal<void(IHandler::buttons_t)> onDown_;
    boost::signals2::signal<void(IHandler::buttons_t)> onUp_;*/
};
}
