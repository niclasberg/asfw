#include "../catch.hpp"
#include "async/conditional.hpp"
#include "async/just.hpp"
#include "async/receive.hpp"
#include "tmp/type_list.hpp"

namespace 
{
    struct Callback
    {
        void operator()(int) const
        {
            argumentType = CALLED_WITH_INT;
        }

        void operator()(float) const
        {
            argumentType = CALLED_WITH_FLOAT;
        }

        enum ArgumentType
        {
            CALLED_WITH_INT,
            CALLED_WITH_FLOAT,
            NOT_CALLED
        };

        ArgumentType & argumentType;
    };
}

TEST_CASE("Conditional")
{
    SECTION("Should provide a value_types typedef")
    {
        auto sender = async::conditional(
            [] { return true; }, 
            async::justValue(int(), float()), 
            async::justValue(double()));

        using Sender = decltype(sender);
        using ValueTypes = async::SenderValueTypes<Sender, tmp::TypeList, tmp::TypeList>;
        using ErrorTypes = async::SenderErrorTypes<Sender, tmp::TypeList>;

        STATIC_REQUIRE(std::is_same_v<ValueTypes, tmp::TypeList<tmp::TypeList<int, float>, tmp::TypeList<double>>>);
        STATIC_REQUIRE(std::is_same_v<ErrorTypes, tmp::TypeList<>>);
    }

    SECTION("Should provide an error_types typedef")
    {
        auto sender = async::conditional(
            [] { return true; }, 
            async::justError(int()), 
            async::justError(double()));

        using Sender = decltype(sender);
        using ValueTypes = async::SenderValueTypes<Sender, tmp::TypeList, tmp::TypeList>;
        using ErrorTypes = async::SenderErrorTypes<Sender, tmp::TypeList>;

        STATIC_REQUIRE(std::is_same_v<ValueTypes, tmp::TypeList<tmp::TypeList<>>>);
        STATIC_REQUIRE(std::is_same_v<ErrorTypes, tmp::TypeList<int, double>>);
    }

    SECTION("Should call the first sender if the predicate returns true")
    {
        Callback::ArgumentType callArgument = Callback::NOT_CALLED;

        auto op = async::connect(
            async::conditional([] { return true;}, async::justValue(int()), async::justValue(float())),
            async::receiveValue(Callback{callArgument}));
        async::start(op);

        REQUIRE(callArgument == Callback::CALLED_WITH_INT);
    }

    SECTION("Should call the second sender if the predicate returns false")
    {
        Callback::ArgumentType callArgument = Callback::NOT_CALLED;

        auto op = async::connect(
            async::conditional([] { return false;}, async::justValue(int()), async::justValue(float())),
            async::receiveValue(Callback{callArgument}));
        async::start(op);

        REQUIRE(callArgument == Callback::CALLED_WITH_FLOAT);
    }
}