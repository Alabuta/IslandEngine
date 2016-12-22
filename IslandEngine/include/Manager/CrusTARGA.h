/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 11th April 2010.
****	Description: TARGA image loader header file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_TARGA_H                // Include guard "CrusTARGA.h"
#define CRUS_TARGA_H

namespace isle {
struct Image {
    int32 bpp_{0};
    uint32 width_{0}, height_{0};
    uint32 format_{0}, type_{0};

    std::vector<byte> data_;

    uint8 BytesPerPixel() const;
};

bool LoadTARGA(Image &image, std::string const &name);
};

#endif // CRUS_TARGA_H