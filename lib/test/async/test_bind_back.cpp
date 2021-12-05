#include "../catch.hpp"
#include "async/bind_back.hpp"

TEST_CASE("Bind back")
{
    SECTION("Should be rvalue callable")
    {
        int arg1 = -1;
        char arg2 = 0;

        async::bindBack(
            [&](int a, char b) { arg1 = a; arg2 = b; }, 
            'c')(1);
        
        REQUIRE(arg1 == 1);
        REQUIRE(arg2 == 'c');
    }

    SECTION("Should be lvalue callable")
    {
        int arg1 = -1;
        char arg2 = 0;

        auto f = async::bindBack(
            [&](int a, char b) { arg1 = a; arg2 = b; }, 
            'c');
        f(1);
        
        REQUIRE(arg1 == 1);
        REQUIRE(arg2 == 'c');
    }

    SECTION("Should be callable for a const lvalue")
    {
        int arg1 = -1;
        char arg2 = 0;

        const auto f = async::bindBack(
            [&](int a, char b) { arg1 = a; arg2 = b; }, 
            'c');
        f(1);
        
        REQUIRE(arg1 == 1);
        REQUIRE(arg2 == 'c');
    }

    SECTION("Should have a correct return value when invoked for an lvalue")
    {
        auto f = async::bindBack(
            [&](int a, int b) 
            { 
                return a + b;
            }, 
            10);

        auto retval = f(100);
        
        REQUIRE(retval == 110);
    }

    SECTION("Should have a correct return value when invoked for an rvalue")
    {
        auto retval = async::bindBack(
            [&](int a, int b) 
            { 
                return a + b;
            }, 
            10)(100);
        
        REQUIRE(retval == 110);
    }

    SECTION("The pipe operator should be rvalue callable")
    {
        int arg1 = -1;
        char arg2 = 0;

        1 | async::bindBack(
            [&](int a, char b) { arg1 = a; arg2 = b; }, 
            'c');
        
        REQUIRE(arg1 == 1);
        REQUIRE(arg2 == 'c');
    }

    SECTION("Should be lvalue callable")
    {
        int arg1 = -1;
        char arg2 = 0;

        auto f = async::bindBack(
            [&](int a, char b) { arg1 = a; arg2 = b; }, 
            'c');
        1 | f;
        
        REQUIRE(arg1 == 1);
        REQUIRE(arg2 == 'c');
    }

    SECTION("Should be callable for a const lvalue")
    {
        int arg1 = -1;
        char arg2 = 0;

        const auto f = async::bindBack(
            [&](int a, char b) { arg1 = a; arg2 = b; }, 
            'c');
        1 | f;
        
        REQUIRE(arg1 == 1);
        REQUIRE(arg2 == 'c');
    }

    SECTION("Should have a correct return value when invoked for an lvalue")
    {
        auto f = async::bindBack(
            [&](int a, int b) { return a + b; }, 
            10);

        auto retval = 100 | f;
        
        REQUIRE(retval == 110);
    }

    SECTION("Should have a correct return value when invoked for a const lvalue")
    {
        const auto f = async::bindBack(
            [&](int a, int b) { return a + b; }, 
            10);

        auto retval = 100 | f;
        
        REQUIRE(retval == 110);
    }

    SECTION("Should have a correct return value when invoked for an rvalue")
    {
        auto retval = 100 | async::bindBack(
            [&](int a, int b) { return a + b; }, 
            10);
        
        REQUIRE(retval == 110);
    }

    SECTION("Should be able to chain")
    {
        auto bb = 
            async::bindBack(
                [&](int a, int b) { return float(a + b) + 0.5f; }, 
                10)
            | async::bindBack(
                [&](float a, float b) { return a + b; },
                10.f);
        
        REQUIRE((5 | bb) == 25.5f);
        REQUIRE((5 | std::move(bb)) == 25.5f);
    }
}