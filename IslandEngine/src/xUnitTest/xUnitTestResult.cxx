/****************************************************************************************
****
****    ...
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 24th March 2012.
****    Description: unit test results class.
****
****************************************************************************************/
#include <iostream>
#include <cstdarg>

#include "xUnitTest\xUnitTest.h"
#include "xUnitTest\xUnitTestResult.h"

namespace xUnit
{
Test::Result::Result() : total_(0), failed_(0) {}
Test::Result::~Result() {}

void Test::Result::Add()
{
    ++total_;
}

void Test::Result::Failed(Test const &_test, int32 _line, acstr _msg, ...)
{
    char msg[256];

    {
        va_list ap;
        va_start(ap, _msg);

        vsprintf_s(msg, sizeof(msg), _msg, ap);

        va_end(ap);
    }

    acstr format = "%s(%d) : \"%s::%s.\n";
    printf(format, _test.file_, _line, _test.name_, msg);

    ++failed_;
}

int32 Test::Result::All() const
{
    if(failed_ > 0){
        printf("Failed: %hd out of %hd test(s).\n", failed_, total_);
        return 1;
    }

    printf("Success: %hd suite test%s passed.\n", total_, total_ > 1 ? "s" : "");
    return 0;
}
};