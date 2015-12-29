/****************************************************************************************
****
****    ...
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    27th November 2013 / 23th March 2012.
****    Description: unit test list.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_XUNIT_TEST_LIST_H      // Include guard "xUnitTestLest.h
#define CRUS_XUNIT_TEST_LIST_H

namespace xUnit
{
class Test::List {
private:
    Test *head_, *tail_;

    List();
    ~List();

public:

    void Add(Test *const test);
    void RunAll() const;

    static List &list();
};
};

#endif // CRUS_XUNIT_TEST_LIST_H