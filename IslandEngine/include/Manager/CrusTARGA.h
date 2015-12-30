/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 11th April 2010.
****	Description: TARGA image loader header file.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_TARGA_H                // Include guard "CrusTARGA.h"
#define CRUS_TARGA_H

namespace isle
{
struct Image {
    int32 bpp_{0};
    uint32 w_{0}, h_{0};
    uint32 format_{0}, type_{0};

    std::vector<byte> data_;
};

bool LoadTARGA(Image &image, std::string const &name);
};

#endif // CRUS_TARGA_H