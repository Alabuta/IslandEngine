/********************************************************************************************************************************
****
****    xUnitTest source code.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: unit test list.
****
********************************************************************************************************************************/
#include "xUnitTest\xUnitTest.h"
#include "xUnitTest\xUnitTestList.h"

namespace xUnit
{
Test::List::List() : head_(nullptr), tail_(nullptr) {}

void Test::List::Add(Test *const _test)
{
    if(head_ == nullptr)
        head_ = _test;

    else if(tail_ == nullptr)
        head_->next_ = _test;

    else tail_->next_ = _test;

    tail_ = _test;
}

void Test::List::RunAll() const
{
    Test const *current = head_;

    while(current != nullptr){
        current->RunTest();
        current = current->next_;
    }
}

/*static*/ __declspec(noinline) Test::List &Test::List::list()
{
    static List list;
    return list;
}
};