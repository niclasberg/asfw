#include "../catch.hpp"
#include "async/event.hpp"

TEST_CASE("Event")
{
    volatile async::Event event;
    async::EventEmitter emitter{&event};
    bool handlerCalled = false;
    auto handler = async::makeEventHandler([&]() {
        handlerCalled = true;
    });

    SECTION("Should call the handler if subscribed")
    {
        emitter.subscribe(&handler);
        event.raise();
        REQUIRE(handlerCalled);
    }

    SECTION("Subscribing twice should return false for the second call")
    {
        REQUIRE(emitter.subscribe(&handler));
        REQUIRE(emitter.subscribe(&handler) == false);
    }

    SECTION("Should not call the event handler if unsubscribed")
    {
        emitter.subscribe(&handler);
        emitter.unsubscribe();
        event.raise();
        REQUIRE(!handlerCalled);
    }
}