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
    //delta = last - delta;

    updateHandler_(*this);

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
            updateHandler_ = [] (auto &handler)
            {
                handler.controller_.pan(handler.delta.x, handler.delta.y);
            }; break;

        case (0x01 | 0x02):
            updateHandler_ = [] (auto &handler)
            {
                auto direction = glm::dot(handler.delta, handler.dollyDirection);
                handler.controller_.dolly(glm::length(handler.delta) * direction);
            }; break;

        case 0x01:
            updateHandler_ = [] (auto &handler)
            {
                handler.controller_.rotate(handler.delta.x, handler.delta.y);
            }; break;

        default:
            updateHandler_ = [] (auto &&) { };
            break;
    }
}

void MouseHandler::onUp(IHandler::buttons_t buttons)
{
    updateHandler_ = [] (auto &&) { };
}
}
