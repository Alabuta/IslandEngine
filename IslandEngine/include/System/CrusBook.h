/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 4th June 2010.
****    Description: book log.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_BOOK_H                 // Include guard "CrusBook.h"
#define CRUS_BOOK_H

namespace isle
{
enum class eNOTE : size_t {
    nSEPAR = 0, nEMPTY, nHYPHEN,
    nINFO, nNOTICE,
    nDEBUG,
    nWARN, nERROR,
    nALERT, nCRITIC, nEMERG
};

namespace Book
{
 void Open();
 void Close();

 // The note argument is severity level and some kind of 
 void AddEvent(isle::eNOTE note, acstr str = "\0", ...);
 void NoteTime(isle::eNOTE note, acstr str = "\0");
};
};

#endif // CRUS_BOOK_H