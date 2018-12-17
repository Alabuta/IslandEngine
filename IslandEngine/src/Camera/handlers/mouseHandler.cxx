#include "mouseHandler.hxx"


void MouseHandler::onMove(float x, float y)
{
    delta = glm::vec2{x, y};
    delta = last - delta;

    updateHandler_(*this);

    last = glm::vec2{x, y};
}

void MouseHandler::onWheel(float, float yoffset)
{
    controller_.dolly(std::signbit(yoffset) ? -1.f : 1.f);
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
                handler.controller_.dolly(glm::length(handler.delta) * direction * .1f);
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

void MouseHandler::onUp(IHandler::buttons_t)
{
    updateHandler_ = [] (auto &&) { };
}
