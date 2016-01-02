/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 12th March 2010.
****    Description: ...
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_MOUSE_H                // Include guard "CrusMouse.h"
#define CRUS_MOUSE_H

#include <functional>
#include <map>

#include "System\CrusInput.h"

namespace isle
{
namespace Input
{
class Mouse : Device {
public:
    enum eSTATE {
        nVISIBLE = 0x01, nACTIVE = 0x02,
        nSHIFT = 0x04, nCTRL = 0x08, nMENU = 0x10,
        nLEFT = 0x20, nMIDDLE = 0x40, nRIGHT = 0x80, nWHEEL = 0x100,
        nMAX_VALUE = 0xFFFFFFFF
    };

    explicit Mouse();
    ~Mouse();

    void Setup();
    void Destroy();

    static void CheckKeySync(uint32 key, std::function<void()> fnJustPressed, std::function<void()> fnWhilePressed,
                              std::function<void()> fnJustUnpressed, std::function<void()> fnUnpressed);

    void Process(RAWMOUSE const *const _data);

    void ShowCursor();
    void HideCursor();

    void Release();

    float x() const;
    float y() const;

    bool GetState(Mouse::eSTATE state) const;

private:
    float relat_x_, relat_y_;
    float sense_x_, sense_y_;

    /*float sense_wheel_;
    int16 wheel_;*/

    uint16 state_;

    static std::map<uint32, bool> mouseKeys_;
};

inline float Mouse::x() const
{
    return relat_x_;
}

inline float Mouse::y() const
{
    return relat_y_;
}

inline bool Mouse::GetState(Mouse::eSTATE _state) const
{
    return ((state_ & _state) == 1);
}
};
};

#endif // CRUS_MOUSE_H