/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: unit test results class.
****
********************************************************************************************************************************/
#include <iostream>

#include "xUnitTest\xUnitTest.h"
#include "xUnitTest\xUnitTestResult.h"

namespace xUnit
{
Test::Result::Result() : total_(0), failed_(0) {}

void Test::Result::Add()
{
    ++total_;
}

void Test::Result::Failed(Test const &_test, int32_t _line, std::string &&_msg)
{
    std::cout << _test.file_name_ << "(" << _line << ") : " << _test.class_name_ << " " << _msg << ".\n";

    ++failed_;
}

int32_t Test::Result::All() const
{
    if(failed_ > 0){
        std::cout << "Failed: " << failed_ << " out of " << total_ << " test" << (total_ > 1 ? "s" : "") << ".\n";
        return 1;
    }

    std::cout << "Success: " << total_ << " test" << (total_ > 1 ? "s" : "") << " passed.\n";
    return 0;
}
};