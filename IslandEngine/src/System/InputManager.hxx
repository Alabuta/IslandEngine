#pragma once

#include <map>
#include <vector>


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#include "System/CrusIsland.h"
#include "System/CrusTypes.h"
#include "System/CrusBook.h"

#include "System/MouseInput.hxx"


namespace isle
{
class InputManager final {
public:

    InputManager(HWND hTargetWnd);

    ~InputManager();

    void Process(WPARAM wParam, LPARAM lParam);

    MouseInput &mouse() noexcept { return mouse_; }

private:
    HWND hTargetWnd_;

    std::vector<std::byte> rawData_;

    MouseInput mouse_;
};
}
