/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: unit test declaration.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_XUNIT_TEST_H           // Include guard "xUnitTest.h
#define CRUS_XUNIT_TEST_H

#include <string>

#define UNIT_TEST_HERITABLE_CLASS       class Test;

#define CHECK(e, m)                     xUnit::Check(e, *this, __LINE__, m);
#define CHECK_EQUAL(e, a, m)            xUnit::CheckEqual(e, a, *this, __LINE__, m);

namespace xUnit
{
class Test {
public:
    explicit Test(std::string &&class_name, std::string &&file_name);
    virtual ~Test() = default;

    static int32_t RunAll();

    class Result;
    static Result &results();

private:
    std::string class_name_, file_name_;
    Test const *next_;

    virtual void RunTest() const = 0;

    Test() = delete;
    Test(Test &&) = delete;
    Test(Test const &) = delete;

    Test const &operator= (Test &&) = delete;
    Test const &operator= (Test const &) = delete;

    class List;
};

template<typename T>
T rand();

void Check(bool expression, Test const &test, int32_t line, std::string &&_msg);

template<typename T>
void CheckEqual(T expected, T actual, Test const &test, int32_t line, std::string &&msg);
};

#define UNIT_SUITE_CLASS(name)          UNIT_SOME_TEST(name)
#define UNIT_NAMED_TEST(name)           namespace name {class Test;}\
                                        UNIT_SOME_TEST(name)

#define UNIT_SOME_TEST(name)\
class name::Test final : protected xUnit::Test {\
public:\
    Test() : xUnit::Test(#name, __FILE__) {};\
\
private:\
    void RunTest() const override;\
\
} test##__COUNTER__;\
\
void name::Test::RunTest() const\

#endif // CRUS_XUNIT_TEST_H