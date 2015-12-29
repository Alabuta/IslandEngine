/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    2th July 2013 / 26th June 2013.
****    Description: supporting a gamepad xbox 360 controller.
****
****************************************************************************************/
#include <cstdio>

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"

#include <URLmon.h>
#pragma comment(lib, "Urlmon.lib")

namespace isle
{
struct USBIDs {
    uint32 hex_;
    char name_[256];
};

void FindVendorDeviceID(USBIDs &_vendor, USBIDs &_device)
{
    UNREFERENCED_PARAMETER(_vendor);
    UNREFERENCED_PARAMETER(_device);
#if _CRUS_TEMP_DISABLED
    // If possible, update the file from web.
    URLDownloadToFileW(nullptr, L"http://www.linux-usb.org/usb.ids", L"..\\data\\usb.ids", 0, nullptr);

    FILE *list;
    char str[256];

    fopen_s(&list, "..\\data\\usb.ids", "r");
    if(list == nullptr || ferror(list) != 0){
        Book::AddEvent(NOTE::nERROR, "can't open USBIDs list file.");
        return;
    }

    uint32 vendor, device;
    astr itert = str;

// :COMPILER: just to shut up the compiler.
#pragma warning(push, 3)
#pragma warning(disable: 4127)
    while(true){
#pragma warning(pop)
        itert = fgets(str, 256, list);

        // Until reach the first device class.
        if(strstr(str, "C 00") == str)
            break;

        // Skiping empty and commented-out lines.
        if(isalnum(*itert) == 0)
            continue;

        // Vendor ID.
        sscanf_s(itert, "%xh", &vendor, sizeof(vendor));

        if(vendor != _vendor.hex_)
            continue;

        // Skiping space like characters.
        while(isspace(*++itert) == 0);
        while(isspace(*++itert) != 0);

        strcpy_s(_vendor.name_, sizeof(_vendor.name_), itert);
        _vendor.name_[strlen(itert) - 1] = '\0';

        // Device ID seeking.
// :COMPILER: just to shut up the compiler.
#pragma warning(push, 3)
#pragma warning(disable: 4127)
        while(true){
#pragma warning(pop)
            itert = fgets(str, 256, list);

            if(*itert == '#')
                continue;

            if(isspace(*itert) == 0)
                break;

            sscanf_s(itert, "%xh", &device, sizeof(device));

            if(device != _device.hex_)
                continue;

            while(isspace(*++itert) == 0);
            while(isspace(*++itert) != 0);

            strcpy_s(_device.name_, sizeof(_device.name_), itert);
            _device.name_[strlen(itert) - 1] = '\0';
            break;
        }

        break;
    }

    fclose(list);
#endif
}
};