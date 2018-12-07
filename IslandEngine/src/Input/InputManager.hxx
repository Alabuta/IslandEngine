#pragma once

#include <map>
#include <vector>

#include <GLFW/glfw3.h>


#include "MouseInput.hxx"


namespace isle
{
class InputManager final {
public:

    InputManager(GLFWwindow *hTargetWnd);

    ~InputManager();

    //void Process(WPARAM wParam, LPARAM lParam);

    MouseInput &mouse() noexcept { return mouse_; }

private:
    GLFWwindow *hTargetWnd_;

    std::vector<std::byte> rawData_;

    MouseInput mouse_;
};
}
