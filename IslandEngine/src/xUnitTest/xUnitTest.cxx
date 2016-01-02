/********************************************************************************************************************************
****
****    ...
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 14th March 2012.
****    Description: unit test declaration.
****
********************************************************************************************************************************/
#include <iostream>
#include <random>

#include "xUnitTest\xUnitTest.h"
#include "xUnitTest\xUnitTestList.h"
#include "xUnitTest\xUnitTestResult.h"

namespace xUnit
{
Test::Test(acstr _name, acstr _file) : next_(nullptr), name_(_name), file_(_file)
{
    List::list().Add(this);
}

Test::~Test() {}

/*static*/ __declspec(noinline) Test::Result &Test::results()
{
    static Result result;
    return result;
}

/*static*/ int32 Test::RunAll()
{
    printf("Performing xUnitTest...\n");

    List::list().RunAll();

    return results().All();
}

template<>
__declspec(noinline) int32 rand<int32>()
{
    static std::tr1::mt19937 engine;

    std::tr1::uniform_int<int32> shift(0, static_cast<int32>(sizeof(int32) * 8));
    std::tr1::uniform_int<int32> value(INT_MIN, INT_MAX);

    return value(engine) >> shift(engine);
}

template<>
__declspec(noinline) float rand<float>()
{
    static std::tr1::mt19937 engine;

    //std::tr1::uniform_int<int32> exp(FLT_MIN_EXP, FLT_MAX_EXP);
    //std::tr1::normal_distribution<float> value;

    //return value(engine) * powf(2.0f, static_cast<float>(exp(engine)));

    //std::tr1::uniform_int<int32> sign(0, 1);
    //std::tr1::uniform_real<float> value(0.0f, FLT_MAX);

    //return sign(engine) == 0 ? value(engine) : -value(engine);

    std::tr1::uniform_real<float> value(-1037.8029f, 1401.055061f);
    return value(engine);
}

void Check(bool _expression, Test const &_test, int32 _line, acstr _msg)
{
    Test::results().Add();

    if(_expression) return;

    acstr format = "%s\": the expression is false";
    Test::results().Failed(_test, _line, format, _msg, _expression);
}

template<>
void CheckEqual<int32>(int32 _expected, int32 _actual, Test const &_test, int32 _line, acstr _msg)
{
    Test::results().Add();

    if(_expected == _actual) return;

    acstr format = "%s\": expected %d but was %d";
    Test::results().Failed(_test, _line, format, _msg, _expected, _actual);
}

template<>
void CheckEqual<float>(float _expected, float _actual, Test const &_test, int32 _line, acstr _msg)
{
    Test::results().Add();

    // :TODO: floats compare...
    if(fabsf((_expected - _actual) / (_actual != 0.0f ? _actual : 1.0f)) < FLT_EPSILON)
        return;

    acstr format = "%s\": expected %gf but was %gf";
    Test::results().Failed(_test, _line, format, _msg, _expected, _actual);
}
}