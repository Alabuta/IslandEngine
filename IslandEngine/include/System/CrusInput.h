/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 7th July 2012.
****    Description: controllers are used in the engine.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_INPUT_H                // Include guard "CrusInput.h"
#define CRUS_INPUT_H

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef  UNICODE
#define  UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*
http://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx
http://msdn.microsoft.com/en-us/library/windows/desktop/ee417014(v=vs.85).aspx
http://msdn.microsoft.com/en-us/library/windows/apps/hh452799.aspx
http://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input
http://www.codeproject.com/Articles/17123/Using-Raw-Input-from-C-to-handle-multiple-keyboard#_Toc156395976
http://www.codeproject.com/Articles/26949/Xbox-360-Controller-Input-in-C-with-XInput
http://www.retroremakes.com/remaketalk/index.php?p=/discussion/1025/getting-the-human-readable-name-of-a-hid-/p1
https://cs.senecac.on.ca/~gam670/pages/content/force.html
http://www.yaldex.com/game-programming/0131020099_ch05lev1sec5.html
http://flylib.com/books/en/1.122.1.92/1/
http://www.cyberforum.ru/cpp-beginners/thread400023.html
http://developer.x-plane.com/?article=obj8-file-format-specification
*/

namespace isle
{
namespace Input
{
class Device {
private:
    // :WARNING: "hDevice_" and "inst_".
    // Do not change the order of this members.
    HANDLE hDevice_;
    Device *inst_;

public:

    /*explicit*/ Device() : hDevice_(nullptr), inst_(nullptr) {};
    virtual ~Device() {};

    void Init(Device *inst);

    void operator= (Device *);

    void Release();

    float x() const;
    float y() const;
};

class Controller {
private:
    ;

public:

    explicit Controller(){};
    virtual ~Controller(){};

    float x() const;
    float y() const;
};

void Setup();
void Destroy();

void DeviceChanged(WPARAM wParam, LPARAM lParam);
void Process(WPARAM wParam, LPARAM lParam);

void Release();
};
};

#endif // CRUS_INPUT_H