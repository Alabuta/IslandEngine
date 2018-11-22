#include "Camera/MouseHandler.hxx"


namespace isle
{
MouseHandler::MouseHandler(OrbitController &controller) : controller_{controller}
{
    ;
}

void MouseHandler::onMove(i64 x, i64 y)
{
    isle::log::Debug() << __FUNCTION__ << ' ' << x << '\t' << y;
}

void MouseHandler::onWheel(i16 delta)
{
    isle::log::Debug() << __FUNCTION__ << ' ' << delta;
}

void MouseHandler::onDown(IHandler::buttons_t buttons)
{
    isle::log::Debug() << __FUNCTION__ << ' ' << std::hex << buttons.to_ulong();
}

void MouseHandler::onUp(IHandler::buttons_t buttons)
{
    isle::log::Debug() << __FUNCTION__ << ' ' << std::hex << buttons.to_ulong();
}
}
