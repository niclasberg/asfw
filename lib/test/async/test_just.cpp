#include "../catch.hpp"
#include "async/just.hpp"
#include "async/receive.hpp"
#include "async/sender.hpp"
#include "tmp/type_list.hpp"

TEST_CASE("JustValue")
{
    SECTION("Should fullfill the sender concept")
    {
        auto sender = async::justValue();

        STATIC_REQUIRE(async::Sender<decltype(sender)>);
    }

    SECTION("Should be constructible from void")
    {
        bool wasCalled = false;
        auto op = async::connect(
            async::justValue(),
            async::receiveValue(
                [&wasCalled]() {
                    wasCalled = true;
                }
            ));
        async::start(op);

        REQUIRE(wasCalled);
    }

    SECTION("Should correctly pass a single argument by value")
    {
        int result = 0;
        auto op = async::connect(
            async::justValue(10),
            async::receiveValue(
                [&result](auto val) {
                    result = val;
                }
            ));
        async::start(op);

        REQUIRE(result == 10);
    }

    SECTION("Should correctly pass a multiple arguments")
    {
        int intResult = 0;
        double doubleResult = 0;
        bool boolResult = false;

        auto op = async::connect(
            async::justValue(10, 20., true),
            async::receiveValue(
                [&](int i, double d, bool b) {
                    intResult = i;
                    doubleResult = d;
                    boolResult = b;
                }
            ));
        async::start(op);

        REQUIRE(intResult == 10);
        REQUIRE(doubleResult == 20.);
        REQUIRE(boolResult == true);
    }

    SECTION("Should accept lvalue argument")
    {
        int input = 10, result = 0;
        auto op = async::connect(
            async::justValue(input),
            async::receiveValue(
                [&result](auto val) {
                    result = val;
                }
            ));
        async::start(op);

        REQUIRE(result == input);
    }

    SECTION("Should provide a value_type typedef")
    {
        auto j = async::justValue(int(10), true, 'c');
        using ValueTypes = async::SenderValueTypes<decltype(j), tmp::TypeList, tmp::TypeList>;
        STATIC_REQUIRE(std::is_same_v<ValueTypes, tmp::TypeList<tmp::TypeList<int, bool, char>>>);
    }
}