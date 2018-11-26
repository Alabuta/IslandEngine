#include <string>
#include <string_view>
#include <unordered_set>

#include <boost/functional/hash_fwd.hpp>

#include "System/InputManager.hxx"

extern "C" {
#include <HIDsdi.h>
#pragma comment(lib, "HID.lib")
}

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace isle
{
InputManager::InputManager(HWND hTargetWnd) : hTargetWnd_{hTargetWnd}
{
    u32 numDevices{0};

    if (GetRawInputDeviceList(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST)) == static_cast<UINT>(-1))
        throw std::runtime_error("failed to get number of devices"s);

    std::vector<RAWINPUTDEVICELIST> list(numDevices);

    if (GetRawInputDeviceList(std::data(list), &numDevices, sizeof(RAWINPUTDEVICELIST)) != numDevices)
        throw std::runtime_error("failed to get device list"s);

    std::map<std::string_view, RAWINPUTDEVICE> const map{
        {
            "mouse"sv,
            RAWINPUTDEVICE{
                HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_MOUSE,
                RIDEV_NOLEGACY | RIDEV_DEVNOTIFY, hTargetWnd
            }
        },
        {
            "keyboard"sv,
            RAWINPUTDEVICE{
                HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_KEYBOARD,
                RIDEV_NOLEGACY | RIDEV_DEVNOTIFY, hTargetWnd
            }
        }
    };

    RID_DEVICE_INFO deviceInfo{sizeof(RID_DEVICE_INFO)};
    u32 size{deviceInfo.cbSize};

    struct hash_t {
        auto operator() (RAWINPUTDEVICE const &rawInputDevice) const noexcept
        {
            std::size_t seed = 0;

            boost::hash_combine(seed, rawInputDevice.usUsagePage);
            boost::hash_combine(seed, rawInputDevice.usUsage);
            boost::hash_combine(seed, rawInputDevice.dwFlags);
            boost::hash_combine(seed, rawInputDevice.hwndTarget);

            return seed;
        }
    };

    struct equal_t {
        auto operator() (RAWINPUTDEVICE const &lhs, RAWINPUTDEVICE const &rhs) const noexcept
        {
            auto equal = true;

            equal = equal && lhs.usUsagePage == rhs.usUsagePage;
            equal = equal && lhs.usUsage == rhs.usUsage;
            equal = equal && lhs.dwFlags == rhs.dwFlags;
            equal = equal && lhs.hwndTarget == rhs.hwndTarget;

            return equal;
        }
    };

    std::unordered_set<RAWINPUTDEVICE, hash_t, equal_t> devices;

    for (auto &&device : list) {
        GetRawInputDeviceInfoW(device.hDevice, RIDI_DEVICEINFO, &deviceInfo, &size);

        switch (device.dwType) {
            case RIM_TYPEMOUSE:
                if (deviceInfo.mouse.dwNumberOfButtons > 1)
                    devices.emplace(map.at("mouse"sv));
                break;

            case RIM_TYPEKEYBOARD:
                //devices.emplace(map.at("keyboard"sv));
                break;

            default:
                break;
        }
    }

    std::vector<RAWINPUTDEVICE> devices_{ std::begin(devices), std::end(devices) };

    if (RegisterRawInputDevices(std::data(devices_), static_cast<u32>(std::size(devices_)), sizeof(RAWINPUTDEVICE)) == FALSE)
        throw std::runtime_error("failed to register input devices"s);
}


InputManager::~InputManager()
{
    u32 numDevices{0};

    if (GetRegisteredRawInputDevices(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST)) == static_cast<UINT>(-1)) {
        log::Error() << "failed to get number of registered devices"s;
        return;
    }

    if (numDevices < 1)
        return;

    std::vector<RAWINPUTDEVICE> list(numDevices);

    if (GetRegisteredRawInputDevices(std::data(list), &numDevices, sizeof(RAWINPUTDEVICELIST)) != numDevices)
        log::Error() << "failed to get registered device list"s;

    else {
        for (auto &&device : list) {
            device.dwFlags = RIDEV_REMOVE;
            device.hwndTarget = nullptr;
        }

        if (RegisterRawInputDevices(std::data(list), static_cast<u32>(std::size(list)),
                                    sizeof(RAWINPUTDEVICE)) == FALSE)
            log::Error() << "failed to unregister of input devices"s;
    }
}

void InputManager::Process(WPARAM wParam, LPARAM lParam)
{
    if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUTSINK)
        return;

    u32 size{0};

    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
                        nullptr, &size, sizeof(RAWINPUTHEADER)) == static_cast<UINT>(-1)) {
        log::Error() << "failed to get input raw data buffer size"s;
        return;
    }

    rawData_.resize(size);

    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
                        std::data(rawData_), &size, sizeof(RAWINPUTHEADER)) == static_cast<UINT>(-1)) {
        log::Error() << "failed to get input data"s;
        return;
    }

    auto rawInput = reinterpret_cast<PRAWINPUT>(std::data(rawData_));

    switch (rawInput->header.dwType) {
        case RIM_TYPEMOUSE:
            mouse_.update(std::move(rawInput->data.mouse));
            break;

        case RIM_TYPEKEYBOARD:
            log::Debug() << "keyboard: "s << rawInput->header.hDevice;
            break;

        case RIM_TYPEHID:
            log::Debug() << "HID: "s << rawInput->header.hDevice;
            break;
    }
}
}
