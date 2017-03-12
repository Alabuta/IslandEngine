/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: unit test results class.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_XUNIT_TEST_RESULT_H    // Include guard "xUnitTestList.h
#define CRUS_XUNIT_TEST_RESULT_H

namespace xUnit
{
class Test::Result {
public:
    explicit Result();

    void Add();
    void Failed(Test const &test, int32_t line, std::string &&msg);

    int32_t All() const;

private:
    int32_t total_, failed_;
};
};

#endif // CRUS_XUNIT_TEST_LIST_H