/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****	Description: TARGA image loader header file.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_TARGA_H                // Include guard "CrusTARGA.h"
#define CRUS_TARGA_H

namespace isle {
struct Image;

bool LoadTARGA(Image *const image, std::string_view name);
};

#endif // CRUS_TARGA_H