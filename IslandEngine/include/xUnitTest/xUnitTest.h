/****************************************************************************************
****
****    ...
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    6th April 2012 / 14th March 2012.
****    Description: unit test declaration.
****
****************************************************************************************/
#pragma once

#ifndef CRUS_XUNIT_TEST_H           // Include guard "xUnitTest.h
#define CRUS_XUNIT_TEST_H

#include <cstddef>

#include "System\CrusIsland.h"
#include "System\CrusTypes.h"

#define UNIT_TEST_HERITABLE_CLASS       class Test;

#define CHECK(e, m)                     xUnit::Check(e, *this, __LINE__, m);
#define CHECK_EQUAL(e, a, m)            xUnit::CheckEqual(e, a, *this, __LINE__, m);

namespace xUnit
{
class Test {
private:
    Test const *next_;
    acstr name_, file_;

    virtual void RunTest() const = 0;
    Test const &operator= (Test const &test) const;

    class List;
    class Result;

public:
    explicit Test(acstr name, acstr file);
    virtual ~Test();

    static int32 RunAll();

    static Result &results();
};

template<typename T>
T rand();

void Check(bool expression, Test const &test, int32 line, acstr msg = "..");

template<typename T>
void CheckEqual(T expected, T actual, Test const &test, int32 line, acstr msg = "..");
};

#define UNIT_SUITE_CLASS(name)          UNIT_SOME_TEST(name)
#define UNIT_NAMED_TEST(name)           namespace name {class Test;}\
                                        UNIT_SOME_TEST(name)

#define UNIT_SOME_TEST(name)\
class name::Test : protected xUnit::Test {\
private:\
    void RunTest() const;\
\
public:\
    Test() : xUnit::Test(#name, __FILE__) {};\
    ~Test(){};\
\
} CRUS_MERGE(Test, __COUNTER__);\
\
void name::Test::RunTest() const\

#endif // CRUS_XUNIT_TEST_H