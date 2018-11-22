#include "Camera/MouseHandler.hxx"


namespace isle
{
MouseHandler::MouseHandler(OrbitController &controller) : controller_{controller}
{
    ;
}

void MouseHandler::onMove(i64 x, i64 y)
{
    delta = glm::vec2{x, y};
    delta = last - delta;

    updateHandler_();

    last = glm::vec2{x, y};
}

void MouseHandler::onWheel(i16 delta)
{
    controller_.dolly(std::signbit(static_cast<float>(delta)) ? -1.f : 1.f);
}

void MouseHandler::onDown(IHandler::buttons_t buttons)
{
    switch (buttons.to_ulong()) {
        case 0x02:
        case 0x04:
            updateHandler_ = [&] { controller_.pan(delta.x, delta.y); };
            break;

        case (0x01 | 0x02):
            updateHandler_ = [&] { controller_.dolly(glm::length(delta) * glm::dot(delta, dollyDirection)); };
            break;

        case 0x01:
            updateHandler_ = [&] { controller_.rotate(delta.x, delta.y); };
            break;

        default:
            updateHandler_ = [] { };
            break;
    }
}

void MouseHandler::onUp(IHandler::buttons_t buttons)
{
    updateHandler_ = [] { };
}
}
