/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 26th July 2012.
****    Description: ...
****
********************************************************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusWindow.h"
#include "System\CrusJoystick.h"

namespace isle
{
namespace Input
{
Joystick::Joystick()
    : x_(0), y_(0), z_(0), Rx_(0), Ry_(0), Rz_(0), slider1_(0), slider2_(0), button_(0)
{
}

Joystick::~Joystick() {};
};
};

/*
PHIDP_PREPARSED_DATA preparsed = nullptr;
PHIDP_BUTTON_CAPS button = nullptr;
PHIDP_VALUE_CAPS value = nullptr;

USAGE usage[24];
bool states[24];

long AxisX;
long AxisY;
long AxisZ;
long AxisRz;
float Throttle;
long Hat;

    preparsed = (PHIDP_PREPARSED_DATA)malloc(64);
    button = (PHIDP_BUTTON_CAPS)malloc(sizeof(HIDP_BUTTON_CAPS));
    value = (PHIDP_VALUE_CAPS)malloc(sizeof(HIDP_VALUE_CAPS));

    if(value != nullptr)
        free(value);

    value = nullptr;

    if(button != nullptr)
        free(button);

    button = nullptr;

    if(preparsed != nullptr)
        free(preparsed);

    preparsed = nullptr;
{
    if(GET_RAWINPUT_CODE_WPARAM(_wParam) == RIM_INPUTSINK)
        return;

    UINT result, size;

    result = GetRawInputData((HRAWINPUT)_lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

    if(result != 0)
        return;

    else if(size > _msize(raw_))
        raw_ = (PRAWINPUT)realloc(raw_, size);

    result = GetRawInputData((HRAWINPUT)_lParam, RID_INPUT, raw_, &size, sizeof(RAWINPUTHEADER));

    if(result == (UINT)-1)
        return;

    if(raw_->header.dwType != RIM_TYPEHID)
        return;

    result = GetRawInputDeviceInfoW(raw_->header.hDevice, RIDI_PREPARSEDDATA, nullptr, &size);

    if(result != 0)
        return;

    else if(size > _msize(preparsed))
        preparsed = (PHIDP_PREPARSED_DATA)realloc(preparsed, size);

    result = GetRawInputDeviceInfoW(raw_->header.hDevice, RIDI_PREPARSEDDATA, preparsed, &size);

    if(result == (UINT)-1)
        return;

    HIDP_CAPS caps;
    if(HidP_GetCaps(preparsed, &caps) == HIDP_STATUS_INVALID_PREPARSED_DATA)
        return;

    uint16 numCaps = caps.NumberInputButtonCaps;
    uint16 valCaps = caps.NumberInputValueCaps;

    if(sizeof(HIDP_BUTTON_CAPS) * numCaps > _msize(button))
        button = (PHIDP_BUTTON_CAPS)realloc(button, sizeof(HIDP_BUTTON_CAPS) * numCaps);

    HidP_GetButtonCaps(HidP_Input, button, &numCaps, preparsed);
    uint16 const numButtons = button[1].Range.UsageMax - button->Range.UsageMin + 1;

    if(sizeof(HIDP_VALUE_CAPS) * valCaps > _msize(value))
        value = (PHIDP_VALUE_CAPS)realloc(value, sizeof(HIDP_VALUE_CAPS) * valCaps);

    HidP_GetValueCaps(HidP_Input, value, &valCaps, preparsed);

    unsigned long uslen = numButtons;
    HidP_GetUsages(HidP_Input, button->UsagePage, 0, usage, &uslen, preparsed,
                   (PCHAR)raw_->data.hid.bRawData, raw_->data.hid.dwSizeHid);

    memset(states, 0, sizeof(states));

    for(unsigned long i = 0; i < uslen; ++i)
        states[usage[i] - button->Range.UsageMin] = true;

    unsigned long val;
    for(unsigned long i = 0; i < caps.NumberInputValueCaps; ++i){
        HidP_GetUsageValue(HidP_Input, value[i].UsagePage, 0, value[i].Range.UsageMin, &val,
                           preparsed, (PCHAR)raw_->data.hid.bRawData, raw_->data.hid.dwSizeHid);

        if(i == 4){
            PHIDP_VALUE_CAPS value1 = &value[i];
            Throttle = (float)val;
        }

        switch(value[i].Range.UsageMin){
            case HID_USAGE_GENERIC_X:
                AxisX = val - value[i].LogicalMax / 2;
                break;

            case HID_USAGE_GENERIC_Y:
                AxisY = val - value[i].LogicalMax / 2;
                break;

            case HID_USAGE_GENERIC_Z:
                AxisZ = val - value[i].LogicalMax / 2;
                break;

            case HID_USAGE_GENERIC_RZ:
                AxisRz = val - value[i].LogicalMax / 2;
                break;

            case HID_USAGE_GENERIC_SLIDER:
                Throttle = val * 100.0f / (float)value[i].LogicalMax;
                break;

            case HID_USAGE_GENERIC_HATSWITCH:
                Hat = val;
                break;
        }
    }

    wchar_t title[256];
    swprintf_s(title, 256, L"X|Y|Rz|Throttle|Hat: %4d|%4d|%4d|%3.4f|%d",
               AxisX, AxisY, AxisRz, Throttle, Hat);

    SetWindowTextW(Window::inst().hWnd(), title);
}
*/