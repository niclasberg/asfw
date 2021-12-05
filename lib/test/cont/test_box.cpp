#include "../catch.hpp"
#include "cont/box.hpp"

bool constructorCalled = true;
bool destructorCalled = false;

struct Pod
{
    Pod(int v) : value(v) { constructorCalled = true; }
    Pod(const Pod & pod) : value(pod.value) {  }
    Pod(Pod && pod) : value(pod.value) {  };
    ~Pod() { destructorCalled = true; }

    int value;
};

TEST_CASE("Box")
{
    constructorCalled = false;
    destructorCalled = false;

    SECTION("Should be able to construct from an rvalue")
    {
        cont::Box<Pod> box;
        
        auto valueRef = box.construct(Pod{5});
        
        REQUIRE(valueRef.value == 5);
        box.destruct();
        REQUIRE(destructorCalled);
    }

    SECTION("Should be able to inplace construct")
    {
        cont::Box<Pod> box;
        
        auto valueRef = box.construct(5);

        REQUIRE(valueRef.value == 5);
        box.destruct();
        REQUIRE(destructorCalled);
    }

    SECTION("Should be able to get a value by lvalue and rvalue reference")
    {
        cont::Box<Pod> box;
        box.construct(Pod{5});

        REQUIRE(box.get().value == 5);
        REQUIRE(std::move(box).get().value == 5);
        
        box.destruct();
        REQUIRE(destructorCalled);
    }

    SECTION("Should copy an lvalue")
    {
        cont::Box<Pod> box;
        Pod value{5};

        auto valueRef = box.construct(value);
        value.value = 8;
        
        REQUIRE(valueRef.value == 5);
        box.destruct();
    }

    SECTION("Should be able to construct from a function object")
    {
        cont::Box<Pod> box;

        auto valueRef = box.constructWith([]() { return Pod{5}; });

        REQUIRE(valueRef.value == 5);
        REQUIRE(constructorCalled);
        
        box.destruct();
        REQUIRE(destructorCalled);
    }
}