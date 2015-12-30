/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: ...
****
****************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "System\CrusMouse.h"

namespace isle
{
namespace Input
{
std::map<uint32, bool> Mouse::mouseKeys_ = {{VK_LBUTTON, false}, {VK_RBUTTON, false}};

Mouse::Mouse()
    : relat_x_(0), relat_y_(0), sense_x_(1.0f), sense_y_(1.0f),
      /*wheel_(0), sense_wheel_(1.0f),*/ state_(nVISIBLE) {}

Mouse::~Mouse() {};

void Mouse::Setup()
{
#if _CRUS_OBSOLETE
    // The tranparent cursor bit mask (slightly larger than is actually needed).
    byte mask[512][512];

    // Just make sure that the everything is okay.
    int32 const w = GetSystemMetrics(SM_CXCURSOR);
    int32 const h = GetSystemMetrics(SM_CYCURSOR);

    memset(&mask[0][0], 0xFF, w * 4);
    memset(&mask[1][0], 0x00, h * 4);

    hCursor = CreateCursor(GetModuleHandle(nullptr), 0, 0, w, h, &mask[0][0], &mask[1][0]);

    if(hCursor == nullptr)
        Book::AddEvent(NOTE::nERROR, "transparent cursor is null.");
#endif
}

void Mouse::Destroy()
{
#if _CRUS_OBSOLETE
    if(hCursor != nullptr)
        DestroyCursor(hCursor);
#endif
}

/*static*/ void Mouse::CheckKeySync(uint32 _key, std::function<void()> _fnJustPressed, std::function<void()> _fnWhilePressed,
                                     std::function<void()> _fnJustUnpressed, std::function<void()> _fnUnpressed)
{
    SHORT const keyState = GetKeyState(_key);

    try {
        if ((keyState & 0x8000) != 0) {
            if (!mouseKeys_[_key])
                _fnJustPressed();

            else _fnWhilePressed();

            mouseKeys_[_key] = true;
        }

        else {
            if (mouseKeys_[_key])
                _fnJustUnpressed();

            else _fnUnpressed();

            mouseKeys_[_key] = false;
        }
    }

    catch (std::out_of_range const &oor) {
        Book::AddEvent(isle::NOTE::nERROR, "Out of Range error: %s", oor.what());
    }
}

void Mouse::Process(RAWMOUSE const *const _data)
{
    state_ |= nACTIVE;

    switch(_data->usFlags){
        case MOUSE_MOVE_RELATIVE:
            relat_x_ = -static_cast<int16>(_data->lLastX) * sense_x_;
            relat_y_ = -static_cast<int16>(_data->lLastY) * sense_y_;
            break;

        case MOUSE_MOVE_ABSOLUTE:
            Book::AddEvent(NOTE::nDEBUG, "MOUSE_MOVE_ABSOLUTE");
            break;

        case MOUSE_VIRTUAL_DESKTOP:             // For a multiple monitor system.
            Book::AddEvent(NOTE::nDEBUG, "MOUSE_VIRTUAL_DESKTOP");
            break;

        case MOUSE_ATTRIBUTES_CHANGED:          // Needs to query the attributes.
            Book::AddEvent(NOTE::nWARN, "MOUSE_ATTRIBUTES_CHANGED");
            break;
    }

    switch(_data->usButtonFlags){
        case RI_MOUSE_LEFT_BUTTON_DOWN:
            state_ |= nLEFT;
            break;

        case RI_MOUSE_LEFT_BUTTON_UP:
            state_ &= nMAX_VALUE - nLEFT;
            break;

        case RI_MOUSE_MIDDLE_BUTTON_DOWN:
            state_ |= nMIDDLE;
            break;

        case RI_MOUSE_MIDDLE_BUTTON_UP:
            state_ &= nMAX_VALUE - nMIDDLE;
            break;

        case RI_MOUSE_RIGHT_BUTTON_DOWN:
            state_ |= nRIGHT;
            break;

        case RI_MOUSE_RIGHT_BUTTON_UP:
            state_ &= nMAX_VALUE - nRIGHT;
            break;

        case RI_MOUSE_WHEEL:
            //wheel_ = _data->usButtonFlags;
            state_ ^= nWHEEL;
            break;
    }

    if((state_ & nVISIBLE) != 0)
        return;

    WINDOWINFO info;
    GetWindowInfo(Window::inst().hWnd(), &info);

    ClipCursor(&info.rcClient);
}

void Mouse::ShowCursor()
{
    if((state_ & nVISIBLE) != 0)
        return;

    ::ShowCursor((state_ ^= nVISIBLE) & nVISIBLE);
    ClipCursor(nullptr);
}

void Mouse::HideCursor()
{
    if((state_ & nVISIBLE) != 0)
        ::ShowCursor((state_ ^= nVISIBLE) & nVISIBLE);
}

void Mouse::Release()
{
    // Disable nACTIVE and nWHEEL with saving state.
    state_ &= nMAX_VALUE - nACTIVE;
    state_ &= nMAX_VALUE - nWHEEL;

    // Resetting mouse movement.
    relat_x_ = 0.0f, relat_y_ = 0.0f;
}
};
};