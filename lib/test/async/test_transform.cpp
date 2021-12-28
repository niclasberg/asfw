#include "../catch.hpp"
#include "async/transform.hpp"
#include "async/just.hpp"
#include "async/receive.hpp"
#include "tmp/type_list.hpp"

TEST_CASE("Transform")
{
    SECTION("Value type should not change for an identity transform")
    {
        auto transformed = async::transform(
            async::justValue(int(5)),
            [](int i) { return i; }
        );
        
        using ValueTypes = async::SenderValueTypes<decltype(transformed), tmp::TypeList, tmp::TypeList>;
        STATIC_REQUIRE(std::is_same_v<ValueTypes, tmp::TypeList<tmp::TypeList<int>>>);
    }

    SECTION("Transform should be able to transform no arguments")
    {
        double receivedValue = 0;
        auto sender = 
            async::transform(
                async::justValue(),
                []() -> double { return 10.0; });

        using ValueTypeList = async::SenderValueTypes<decltype(sender), tmp::TypeList, tmp::TypeList>;
        STATIC_REQUIRE(std::is_same_v<ValueTypeList, tmp::TypeList<tmp::TypeList<double>>>);

        auto op = 
            async::connect(
                std::move(sender),
                async::receiveValue([&receivedValue](auto v) {
                    receivedValue = v;
                }));
        async::start(op);

        REQUIRE(receivedValue == 10.0);
    }

    SECTION("Transform should be able to transform multiple arguments")
    {
        double receivedValue = 0;
        auto sender = 
            async::transform(
                    async::justValue(int(5), true, 'a'),
                    [](auto i, auto, auto) -> double { return i*5; });

        using ValueTypes = async::SenderValueTypes<decltype(sender), tmp::TypeList, tmp::TypeList>;
        STATIC_REQUIRE(std::is_same<ValueTypes, tmp::TypeList<tmp::TypeList<double>>>::value);        
        STATIC_REQUIRE(async::Sender<decltype(sender)>);

        auto op = async::connect(std::move(sender),
                async::receiveValue([&receivedValue](auto v) {
                    receivedValue = v;
                }));
        async::start(op);

        REQUIRE(receivedValue == 25);
    }

    SECTION("Should pass errors through un-transformed")
    {
        const int errorCode = 10;
        int receivedError = 0;
        auto op = 
            async::connect(
                async::transform(
                    async::justError(errorCode),
                    []() { }),
                async::receiveError([&receivedError](auto e) {
                    receivedError = e;
                }));
        async::start(op);

        REQUIRE(receivedError == errorCode);
    }

    SECTION("The factory should support piping")
    {
        int receivedValue = -1;

        auto sender = async::justValue(int(5)) 
                | async::transform([&](int i) { return i*2; })
                | async::transform([&](int i) { return i; });

        STATIC_REQUIRE(async::Sender<decltype(sender)>);
        
        auto op = async::connect(std::move(sender), 
            async::receiveValue([&receivedValue](auto v) {
                receivedValue = v;
            }));
        async::start(op);

        REQUIRE(receivedValue == 10);
    }
}