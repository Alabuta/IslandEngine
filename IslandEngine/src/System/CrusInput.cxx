/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 7th July 2012.
****    Description: controllers are used by the engine.
****
********************************************************************************************************************************/
#include "System\CrusSystem.h"
#include "System\CrusWindow.h"

#include "System\CrusInput.h"
#include "System\CrusGamepad.h"

extern "C" {
#include <HIDsdi.h>
#pragma comment(lib, "HID.lib")
}


namespace {
using namespace isle;
using namespace Input;

size_t const kMAX_HIDS = 16;
size_t const kMAX_NAME = 1024;

Device devices[kMAX_HIDS];

class CInput {
private:
    u32 nHID_;
    u32 raw_size_;

    RAWINPUT *raw_;

    // The indices of HIDs in the "kHID_GENERIC" devices array.
    enum eDEVICE_TYPE {
        nKEYBOARD = 0, nMOUSE, nJOYSTICK, nGAMEPAD
    };

    CInput() : nHID_(0), raw_size_(0), raw_(nullptr) { };
    ~CInput() { };

public:

    /*
        // A game controls.
        enum eACTION_KEYS {
            nFORWARD = 0, nBACK, nLEFT, nRIGHT,
            nACTIVATE, nSPRINT, nCROUCH,
            nACTION_FINAL
        };

        // A game axis.
        enum eAXIS_KEYS {
            nAXIS_X = 0, nAXIS_Y, nAXIS_Z,
            nAXIS_RZ, nTHROTTLE,
            nAXIS_FINAL
        };
    */

    void Setup();
    void Destroy();

    void DeviceChanged(WPARAM wParam, LPARAM lParam);
    void Process(WPARAM wParam, LPARAM lParam);

    static CInput &input();
};

void CInput::Setup()
{
    u32 nDevs = {0};

    if (GetRawInputDeviceList(nullptr, &nDevs, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
        log::Fatal() << "an error is returned: " << __FILE__ << "at line" << __LINE__;

    // Doubled size - just in case.
    RAWINPUTDEVICELIST ridl[kMAX_HIDS * 2];

    if (GetRawInputDeviceList(ridl, &nDevs, sizeof(RAWINPUTDEVICELIST)) != nDevs)
        log::Fatal() << "can't to get list of attached HID devices.";

    // Shorter name, that's reason.
    HWND const hTargetWnd = Window::main().hWnd();

    // Ready templated array of generic desktop controls.
    RAWINPUTDEVICE const kHID_GENERIC[4] = {
        {HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_KEYBOARD, RIDEV_DEVNOTIFY, hTargetWnd},
        {HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_MOUSE,    RIDEV_DEVNOTIFY, hTargetWnd},
        {HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_JOYSTICK, RIDEV_DEVNOTIFY, hTargetWnd},
        {HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_GAMEPAD,  RIDEV_DEVNOTIFY, hTargetWnd}
    };

    RAWINPUTDEVICE rid[kMAX_HIDS] = {0};
    UINT size = sizeof(RID_DEVICE_INFO);

    RID_DEVICE_INFO rdi = {
        sizeof(RID_DEVICE_INFO), 0, 0
    };

    eDEVICE_TYPE dev_type = nKEYBOARD;
    //u32 gpad_num = 0;

    // Checking all plugged HID devices.
    for (u32 i = 0; i < nDevs; ++i) {
        GetRawInputDeviceInfoW(ridl[i].hDevice, RIDI_DEVICEINFO, &rdi, &size);

        switch (ridl[i].dwType) {
#if _CRUS_TEMP_DISABLED
            case RIM_TYPEKEYBOARD:
                if (rdi.keyboard.dwType == 0x51)     // Ignoring system keyboard.
                    continue;

                dev_type = nKEYBOARD;
                break;

            case RIM_TYPEMOUSE:
                if (rdi.mouse.dwNumberOfButtons < 3)
                    continue;

                dev_type = nMOUSE;
                break;
#endif
            case RIM_TYPEHID:
                if (rdi.hid.usUsagePage != HID_USAGE_PAGE_GENERIC)
                    continue;

                switch (rdi.hid.usUsage) {
                    // XBox 360 (or like) controllers.
                    case HID_USAGE_GENERIC_GAMEPAD:
                        dev_type = nGAMEPAD;
                        log::Debug() << "gpad: " << ridl[i].hDevice;
                        break;

                        // Other HID devices (joysticks, gamepads etc).
                    case HID_USAGE_GENERIC_JOYSTICK:
                        dev_type = nJOYSTICK;
                        log::Debug() << "joy: " << ridl[i].hDevice;
                        break;
                } break;

            default:
                continue;
        }

        memcpy(&rid[nHID_++], &kHID_GENERIC[dev_type], sizeof(RAWINPUTDEVICE));
    }

    if (nHID_ < 1)
        return;

    if (RegisterRawInputDevices(rid, nHID_, sizeof(RAWINPUTDEVICE)) == FALSE)
        log::Fatal() << "failed to register types of HID devices.";

    if (GetRegisteredRawInputDevices(nullptr, &nHID_, sizeof(RAWINPUTDEVICE)) == (UINT)-1)
        log::Fatal() << "can't get number of registered types of HID devices.";

    log::Info() << nHID_ << "types of HID device(s) is registered.";
}

void CInput::Destroy()
{
    if (GetRegisteredRawInputDevices(nullptr, &nHID_, sizeof(RAWINPUTDEVICE)) == (UINT)-1)
        log::Fatal() << "an error is returned: " << __FILE__ << "at line" << __LINE__;

    if (nHID_ < 1)
        return;

    RAWINPUTDEVICE rid[kMAX_HIDS];

    if (GetRegisteredRawInputDevices(rid, &nHID_, sizeof(RAWINPUTDEVICE)) != nHID_)
        log::Error() << "failed to get list of registered HID devices.";

    else {
        for (u32 i = 0; i < nHID_; ++i) {
            rid[i].dwFlags = RIDEV_REMOVE;
            rid[i].hwndTarget = nullptr;
        }

        if (RegisterRawInputDevices(rid, nHID_, sizeof(RAWINPUTDEVICE)) == FALSE)
            log::Fatal() << "failed to unregister of HID devices.";
    }

    if (raw_ != nullptr)
        free(raw_);
    raw_ = nullptr;
}

void CInput::DeviceChanged(WPARAM, LPARAM _lParam)
{
    if (_lParam == NULL/* || raw_ == nullptr*/) return;

    /*{
        char temp[256] = "NULL";

        UINT size = sizeof(temp);
        GetRawInputDeviceInfoA(reinterpret_cast<HANDLE>(_lParam), RIDI_DEVICENAME, temp, &size);

        if(strstr(temp, "\\\\?\\HID#") != temp && strstr(temp, "\\\\?\\ACPI#") != temp)
            return;

        if(strstr(temp, "Vid_8888") != nullptr)
            return;

        log::Debug() << "%s", temp);
    }*/

    RID_DEVICE_INFO rdi = {
        sizeof(RID_DEVICE_INFO), 0, 0
    };

    {
        UINT size = sizeof(RID_DEVICE_INFO);
        GetRawInputDeviceInfoW(reinterpret_cast<HANDLE>(_lParam), RIDI_DEVICEINFO, &rdi, &size);
    }

    switch (rdi.dwType) {
#if _CRUS_TEMP_DISABLED
        case RIM_TYPEMOUSE:
            break;

        case RIM_TYPEKEYBOARD:
            break;
#endif
        case RIM_TYPEHID:
            if (rdi.hid.usUsagePage != HID_USAGE_PAGE_GENERIC)
                break;

            switch (rdi.hid.usUsage) {
                case HID_USAGE_GENERIC_JOYSTICK:
                    break;

                case HID_USAGE_GENERIC_GAMEPAD:
                    break;
            } break;
    }

    /*if(rdi.dwType == RIM_TYPEHID && rdi.hid.usUsagePage != HID_USAGE_PAGE_GENERIC)
        return;*/

        /*if(GET_DEVICE_CHANGE_WPARAM(_wParam) == GIDC_ARRIVAL)
            log::Debug() << "A new device has been added to the system.");

        else if(GET_DEVICE_CHANGE_WPARAM(_wParam) == GIDC_REMOVAL)
            log::Debug() << "A device has been removed from the system.");*/
}

void CInput::Process(WPARAM _wParam, LPARAM _lParam)
{
    if (GET_RAWINPUT_CODE_WPARAM(_wParam) == RIM_INPUTSINK)
        return;

    UINT size = 0;

    // Getting size of incoming raw data.
    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(_lParam), RID_INPUT, nullptr,
        &size, sizeof(RAWINPUTHEADER)) == (UINT)-1)
        return;

    if (size > raw_size_) {
        raw_ = (PRAWINPUT)realloc(raw_, static_cast<size_t>(raw_size_ = size));

        if (raw_ == nullptr)
            return;
    }

    // Getting raw data from RAWINPUT structure.
    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(_lParam), RID_INPUT, raw_,
        &size, sizeof(RAWINPUTHEADER)) == (UINT)-1)
        return;

    switch (raw_->header.dwType) {
        case RIM_TYPEMOUSE:
            log::Debug() << raw_->header.hDevice;
            break;

        case RIM_TYPEKEYBOARD:
            log::Debug() << raw_->header.hDevice;
            break;

        case RIM_TYPEHID:
            log::Debug() << raw_->header.hDevice;
            break;
    }
}














/*static*/ __declspec(noinline) CInput &CInput::input()
{
    static CInput input;
    return input;
}
};

namespace isle {
namespace Input {
float Device::x() const
{
    return 1.0f;
}

float Device::y() const
{
    return 1.0f;
}

/*template<class Derived>*/
/*extern*/ __forceinline void Device/*<Derived>*/::Release()
{
    //static_cast<Derived *>(this)->Release();
}

template<class T>
/*extern*/ __forceinline void Release(/*Device<T> &_device*/)
{
    _device.Release();
}

extern __forceinline void Input::Setup()
{
    CInput::input().Setup();
}

extern __forceinline void Input::Destroy()
{
    CInput::input().Destroy();
}

extern __forceinline void Input::DeviceChanged(WPARAM _wParam, LPARAM _lParam)
{
    CInput::input().DeviceChanged(_wParam, _lParam);
}

extern __forceinline void Input::Process(WPARAM _wParam, LPARAM _lParam)
{
    CInput::input().Process(_wParam, _lParam);
}

extern __forceinline void Input::Release()
{
}
};
};
