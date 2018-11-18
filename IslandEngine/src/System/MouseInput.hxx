#pragma once

#include <memory>
#include <bitset>

#include <boost/signals2.hpp>


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "System/CrusIsland.h"
#include "System/CrusTypes.h"
#include "System/CrusBook.h"

namespace isle
{
class MouseInput final {
public:

    class IHandler {
    public:

        using buttons_t = std::bitset<5>;

        virtual ~IHandler() = default;

        virtual void onMove(i32 x, i32 y) = 0;
        virtual void onWheel(i32 delta) = 0;
        virtual void onDown(buttons_t buttons) = 0;
        virtual void onUp(buttons_t buttons) = 0;
    };

    void connect(std::weak_ptr<IHandler> slot);

    void update(RAWMOUSE &&data);

private:

    std::bitset<5> buttonsDown_, buttonsUp_;

    boost::signals2::signal<void(i32, i32)> onMove_;
    boost::signals2::signal<void(i32)> onWheel_;
    boost::signals2::signal<void(IHandler::buttons_t)> onDown_;
    boost::signals2::signal<void(IHandler::buttons_t)> onUp_;
};
}
