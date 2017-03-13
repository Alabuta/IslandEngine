/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: unit test declaration.
****
********************************************************************************************************************************/
#include <iostream>
#include <limits>
#include <random>
#include <sstream>

#include "xUnitTest\xUnitTest.h"
#include "xUnitTest\xUnitTestList.h"
#include "xUnitTest\xUnitTestResult.h"

namespace {
auto constexpr kBAD_STRING = "<bad string expression>";
}

namespace xUnit
{
Test::Test(std::string &&_class_name, std::string &&_file_name) : next_(nullptr), class_name_(_class_name), file_name_(_file_name)
{
    List::list().Add(this);
}

/*static*/ __declspec(noinline) Test::Result &Test::results()
{
    static Result result;
    return result;
}

/*static*/ int32_t Test::RunAll()
{
    std::cout << "Performing xUnitTest...\n";

    List::list().RunAll();

    return results().All();
}

template<>
__declspec(noinline) int32_t rand<int32_t>()
{
    // The seed.
    static std::random_device rd;
    // Mersenne-Twister engine.
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> value(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());

    return value(rd);
}

template<>
__declspec(noinline) float rand<float>()
{
    // The seed.
    static std::random_device rd;
    // Mersenne-Twister engine.
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> value(-1000000.f, 1000000.f);

    return value(rd);
}

void Check(bool _expression, Test const &_test, int32_t _line, std::string &&_msg)
{
    Test::results().Add();

    if(_expression) return;

    std::ostringstream msg(_msg);
    msg << ": false expression";

    Test::results().Failed(_test, _line, msg.good() ? msg.str() : kBAD_STRING);
}

template<>
void CheckEqual<int32_t>(int32_t _expected, int32_t _actual, Test const &_test, int32_t _line, std::string &&_msg)
{
    Test::results().Add();

    if(_expected == _actual) return;

    std::ostringstream msg(_msg);
    msg << ": expected " << _expected << " instead " << _actual;

    Test::results().Failed(_test, _line, msg.good() ? msg.str() : kBAD_STRING);
}

template<>
void CheckEqual<float>(float _expected, float _actual, Test const &_test, int32_t _line, std::string &&_msg)
{
    Test::results().Add();

    // :TODO: floats compare...
    if (std::abs((_expected - _actual) / (_actual != 0.0f ? _actual : 1.0f)) < std::numeric_limits<float>::epsilon())
        return;

    std::ostringstream msg(_msg);
    msg << ": expected " << _expected << " instead " << _actual;

    Test::results().Failed(_test, _line, msg.good() ? msg.str() : kBAD_STRING);
}
}