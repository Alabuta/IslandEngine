/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: unit test list.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_XUNIT_TEST_LIST_H      // Include guard "xUnitTestLest.h
#define CRUS_XUNIT_TEST_LIST_H

namespace xUnit
{
class Test::List {
public:

    void Add(Test *const test);
    void RunAll() const;

    static List &list();

private:
    Test *head_, *tail_;

    explicit List();
};
};

#endif // CRUS_XUNIT_TEST_LIST_H