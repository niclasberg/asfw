#include "catch.hpp"
#include "delegate.hpp"

int freeFunction(char, int b)
{
    return b;
}

struct Foo
{
    enum FunctionCalled
    {
        NONE,
        VOID_MEMBER,
        CONST_VOID_MEMBER
    };

    void voidMember() { functionCalled = VOID_MEMBER; }
    void constVoidMember() const { functionCalled = CONST_VOID_MEMBER; }

    mutable FunctionCalled functionCalled = NONE;
};

TEST_CASE("Delegate")
{
    SECTION("Bool conversion operator should return false for a default constructed delegate")
    {
        Delegate<void()> d;
        REQUIRE(!bool(d));
    }

    SECTION("Void member function pointer")
    {
        Foo f;
        Delegate d{memFn<&Foo::voidMember>, f};

        d();

        REQUIRE(f.functionCalled == Foo::VOID_MEMBER);
    }

    SECTION("Const member function pointer")
    {
        Foo f;
        Delegate d{memFn<&Foo::constVoidMember>, f};

        d();

        REQUIRE(f.functionCalled == Foo::CONST_VOID_MEMBER);
    }

    SECTION("Free function")
    {
        Delegate d{fn<&freeFunction>};

        auto result = d('a', 10);

        REQUIRE(result == 10);
    }

    SECTION("Should support copy construction")
    {
        Delegate d{fn<&freeFunction>};
        auto d2 = d;

        auto result = d2('a', 10);

        REQUIRE(result == 10);
    }

    SECTION("Should support copy assignment")
    {
        Delegate d{fn<&freeFunction>};
        Delegate<int(char, int)> d2;
        d2 = d;

        auto result = d2('a', 10);

        REQUIRE(result == 10);
    }

    SECTION("Should support lambdas")
    {
        auto f = [](int i) -> bool { return i > 15; };
        Delegate<bool(int)> d{&f};

        auto result = d(10);

        REQUIRE(result == false);
    }
}