/****************************************************************************************
****
****    ...
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    24th March 2012 / 24th March 2012.
****    Description: unit test results class.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_XUNIT_TEST_RESULT_H    // Include guard "xUnitTestLest.h
#define CRUS_XUNIT_TEST_RESULT_H

namespace xUnit
{
class Test::Result {
private:
    int32 total_, failed_;

public:
    Result();
    ~Result();

    void Add();
    void Failed(Test const &test, int32 line, acstr msg, ...);

    int32 All() const;
};
};

#endif // CRUS_XUNIT_TEST_LIST_H