/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    30th November 2013 / 4th June 2010.
****    Description: book log.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_BOOK_H                 // Include guard "CrusBook.h"
#define CRUS_BOOK_H

namespace isle
{
namespace NOTE {
    enum eNOTE {
        nSEPAR = 0, nEMPTY, nHYPHEN,
        nINFO, nNOTICE,
        nDEBUG,
        nWARN, nERROR,
        nALERT, nCRITIC, nEMERG
    };
};

namespace Book
{
 void Open();
 void Close();

 // The note argument is severity level and some kind of 
 void AddEvent(isle::NOTE::eNOTE note, acstr str = "\0", ...);
 void NoteTime(isle::NOTE::eNOTE note, acstr str = "\0");
};
};

#endif // CRUS_BOOK_H