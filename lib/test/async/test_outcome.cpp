#include "../catch.hpp"
#include "async/outcome.hpp"
#include <type_traits>

using namespace async;

struct Dummy {};
struct NonTriviallyDestructible
{
    ~NonTriviallyDestructible() { }
};

TEST_CASE("Destructor should be trivial if the contained values are")
{
    STATIC_REQUIRE(std::is_trivially_destructible_v<Outcome<void, void>>);
    STATIC_REQUIRE(std::is_trivially_destructible_v<Outcome<int, int>>);
    STATIC_REQUIRE(std::is_trivially_destructible_v<Outcome<void, int>>);
    STATIC_REQUIRE(std::is_trivially_destructible_v<Outcome<int, void>>);
    STATIC_REQUIRE(!std::is_trivially_destructible_v<Outcome<NonTriviallyDestructible, NonTriviallyDestructible>>);
    STATIC_REQUIRE(!std::is_trivially_destructible_v<Outcome<void, NonTriviallyDestructible>>);
    STATIC_REQUIRE(!std::is_trivially_destructible_v<Outcome<NonTriviallyDestructible, void>>);
}

TEST_CASE("Outcome should be copy constructible")
{
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<void, void>>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<int, int>>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<void, int>>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<int, void>>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<NonTriviallyDestructible, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<int, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<NonTriviallyDestructible, int>>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<void, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Outcome<NonTriviallyDestructible, void>>);
}

TEST_CASE("Outcome should be move constructible")
{
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<void, void>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<int, int>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<void, int>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<int, void>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<NonTriviallyDestructible, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<int, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<NonTriviallyDestructible, int>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<void, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Outcome<NonTriviallyDestructible, void>>);
}

TEST_CASE("Outcome should be copy assignable")
{
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<void, void>>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<int, int>>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<void, int>>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<int, void>>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<NonTriviallyDestructible, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<int, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<NonTriviallyDestructible, int>>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<void, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Outcome<NonTriviallyDestructible, void>>);
}

TEST_CASE("Outcome should be move assignable")
{
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<void, void>>);
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<int, int>>);
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<void, int>>);
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<int, void>>);
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<NonTriviallyDestructible, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<int, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<NonTriviallyDestructible, int>>);
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<void, NonTriviallyDestructible>>);
    STATIC_REQUIRE(std::is_move_assignable_v<Outcome<NonTriviallyDestructible, void>>);
}

template<class F>
struct DestructorCallback
{
    DestructorCallback(const F & f) : f_(f) {}
    DestructorCallback(const DestructorCallback &) = default;
    DestructorCallback(DestructorCallback &&) = default;
    DestructorCallback &operator=(const DestructorCallback &) = default;
    DestructorCallback &operator=(DestructorCallback &&) = default;
    ~DestructorCallback() { f_(); }
    F f_;
};

TEST_CASE("Outcome should call the destructor for non-trivial types")
{
    int destructorCalled = 0;
    auto callback = [&]() { destructorCalled += 1; };
    using C = DestructorCallback<decltype(callback)>;

    SECTION("Ok value, trivially destructible error")
    {
        {
            Outcome<C, double>{success, callback};
        }
        REQUIRE(destructorCalled == 1);
    }

    SECTION("Ok value, non-trivially destructible error")
    {
        {
            Outcome<C, NonTriviallyDestructible>{success, callback};
        }
        REQUIRE(destructorCalled == 1);
    }

    SECTION("Ok value, void error")
    {
        {
            Outcome<C, void>{success, callback};
        }
        REQUIRE(destructorCalled == 1);
    }

    SECTION("Error value, trivially destructible value")
    {
        {
            Outcome<double, C>{err, callback};
        }
        REQUIRE(destructorCalled == 1);
    }

    SECTION("Error value, non-trivially destructible error")
    {
        {
            Outcome<NonTriviallyDestructible, C>{err, callback};
        }
        REQUIRE(destructorCalled == 1);
    }

    SECTION("Error value, void error")
    {
        {
            Outcome<void, C>{err, callback};
        }
        REQUIRE(destructorCalled == 1);
    }
}

template<class F>
struct MoveConstructorCallback
{
    MoveConstructorCallback(const F & f) : f_(f) {}
    MoveConstructorCallback(const MoveConstructorCallback &) = default;
    MoveConstructorCallback(MoveConstructorCallback && rhs) : f_(rhs.f_)
    {
        f_();
    }
    MoveConstructorCallback &operator=(const MoveConstructorCallback &) = default;
    MoveConstructorCallback &operator=(MoveConstructorCallback &&) = default;
    ~MoveConstructorCallback() { }
    F f_;
};

TEST_CASE("Move constructor should call the move-constructor of the underlying type")
{
    int moveConstructorCalled = 0;
    auto callback = [&]() { moveConstructorCalled += 1; };
    using C = MoveConstructorCallback<decltype(callback)>;

    SECTION("Ok value, trivially destructible error")
    {
        Outcome<C, double> x{success, callback};
        auto _ = std::move(x);
        REQUIRE(moveConstructorCalled == 1);
    }

    SECTION("Ok value, non-trivially destructible error")
    {
        Outcome<C, NonTriviallyDestructible> x{success, callback};
        auto _ = std::move(x);
        REQUIRE(moveConstructorCalled == 1);
    }

    SECTION("Ok value, void error")
    {
        Outcome<C, void> x{success, callback};
        auto _ = std::move(x);
        REQUIRE(moveConstructorCalled == 1);
    }

    SECTION("Error value, trivially destructible value")
    {
        Outcome<double, C> x{err, callback};
        auto _ = std::move(x);
        REQUIRE(moveConstructorCalled == 1);
    }

    SECTION("Error value, non-trivially destructible error")
    {
        Outcome<NonTriviallyDestructible, C> x{err, callback};
        auto _ = std::move(x);
        REQUIRE(moveConstructorCalled == 1);
    }

    SECTION("Error value, void error")
    {
        Outcome<void, C> x{err, callback};
        auto _ = std::move(x);
        REQUIRE(moveConstructorCalled == 1);
    }
}

TEST_CASE("Outcome::isSuccess")
{
    SECTION("isSuccess should be false if constructed with an error")
    {
        constexpr auto outcome = Outcome<Dummy, int>{err, 10};
        STATIC_REQUIRE(outcome.isSuccess() == false);
        STATIC_REQUIRE(outcome.isError() == true);
        STATIC_REQUIRE(outcome.isDone() == false);
        STATIC_REQUIRE(outcome.error() == 10);
    }

    SECTION("isSuccess should be true if constructed with an ok")
    {
        constexpr auto outcome = Outcome<int, Dummy>{success, 10};
        STATIC_REQUIRE(outcome.isSuccess() == true);
        STATIC_REQUIRE(outcome.isError() == false);
        STATIC_REQUIRE(outcome.isDone() == false);
        STATIC_REQUIRE(outcome.value() == 10);
    }

    SECTION("Should support void value type")
    {
        STATIC_REQUIRE(Outcome<void, int>{success}.isSuccess());
        STATIC_REQUIRE(!Outcome<void, int>{err, 10}.isSuccess());
    }
}

TEST_CASE("Outcome comparison")
{
    SECTION("Done")
    {
        STATIC_REQUIRE(Outcome<void, void>{done} == Outcome<void, void>{done});
        STATIC_REQUIRE(Outcome<int, void>{done} == Outcome<int, void>{done});
        STATIC_REQUIRE(Outcome<void, int>{done} == Outcome<void, int>{done});
        STATIC_REQUIRE(Outcome<int, int>{done} == Outcome<int, int>{done});
    }

    SECTION("Success")
    {
        STATIC_REQUIRE(Outcome<void, void>{success} == Outcome<void, void>{success});
        STATIC_REQUIRE(Outcome<int, void>{success, 5} == Outcome<int, void>{success, 5});
        STATIC_REQUIRE(Outcome<int, void>{success, 5} != Outcome<int, void>{success, 3});
        STATIC_REQUIRE(Outcome<void, int>{success} == Outcome<void, int>{success});
        STATIC_REQUIRE(Outcome<int, int>{success, 10} == Outcome<int, int>{success, 10});
        STATIC_REQUIRE(Outcome<int, int>{success, 10} != Outcome<int, int>{success, 7});
    }

    SECTION("Error")
    {
        STATIC_REQUIRE(Outcome<void, void>{err} == Outcome<void, void>{err});
        STATIC_REQUIRE(Outcome<void, int>{err, 5} == Outcome<void, int>{err, 5});
        STATIC_REQUIRE(Outcome<void, int>{err, 5} != Outcome<void, int>{err, 3});
        STATIC_REQUIRE(Outcome<int, void>{err} == Outcome<int, void>{err});
        STATIC_REQUIRE(Outcome<int, int>{err, 10} == Outcome<int, int>{err, 10});
        STATIC_REQUIRE(Outcome<int, int>{err, 10} != Outcome<int, int>{err, 7});
    }

    SECTION("Should be equal after copy <int, void>")
    {
        auto a = Outcome<int, void>{success, 10};
        auto b = a;
        REQUIRE(a == b);
    }

    SECTION("Should be equal after copy <void, int>")
    {
        auto a = Outcome<void, int>{err, 10};
        auto b = a;
        REQUIRE(a == b);
    }
}

/*TEST_CASE("map with Outcome should transform Outcomes")
{
    STATIC_REQUIRE(
        map(
            Outcome<double, int>{ok, 20.0}, 
            [](double x) { return x * 2.0; }
        )
        ==
        Outcome<double, int>{ok, 40.0}
    );

    STATIC_REQUIRE(
        map(
            Outcome<double, int>{err, 10}, 
            [](double) { return 'c'; }
        )
        ==
        Outcome<char, int>{err, 10}
    );

    STATIC_REQUIRE(
        map(
            Outcome<double, int>{ok, 20.0}, 
            [](double) { }
        )
        ==
        Outcome<void, int>{ok}
    );

    STATIC_REQUIRE(
        map(
            Outcome<double, int>{err, 10}, 
            [](double) { }
        )
        ==
        Outcome<void, int>{err, 10}
    );

    STATIC_REQUIRE(
        map(
            Outcome<double, void>{ok, 20.0}, 
            [](double x) { return x * 2.0; }
        )
        ==
        Outcome<double, void>{ok, 40.0}
    );

    STATIC_REQUIRE(
        map(
            Outcome<double, void>{err}, 
            [](double x) { return x * 2.0; }
        )
        ==
        Outcome<double, void>{err}
    );

    STATIC_REQUIRE(
        map(
            Outcome<void, int>{ok}, 
            []() { return 10; }
        )
        ==
        Outcome<int, int>{ok, 10}
    );

    STATIC_REQUIRE(
        map(
            Outcome<void, int>{err, 10}, 
            []() { return 10; }
        )
        ==
        Outcome<int, int>{err, 10}
    );
}

TEST_CASE("mapError with Outcome should transform errors")
{
    STATIC_REQUIRE(
        mapError(
            Outcome<int, double>{err, 20.0}, 
            [](double x) { return x * 2.0; }
        )
        ==
        Outcome<int, double>{err, 40.0}
    );

    STATIC_REQUIRE(
        mapError(
            Outcome<int, double>{ok, 10}, 
            [](double) { return 'c'; }
        )
        ==
        Outcome<int, char>{ok, 10}
    );

    STATIC_REQUIRE(
        mapError(
            Outcome<int, double>{err, 20.0}, 
            [](double) { }
        )
        ==
        Outcome<int, void>{err}
    );

    STATIC_REQUIRE(
        mapError(
            Outcome<int, double>{ok, 10}, 
            [](double) { }
        )
        ==
        Outcome<int, void>{ok, 10}
    );

    STATIC_REQUIRE(
        mapError(
            Outcome<void, double>{err, 20.0}, 
            [](double x) { return x * 2.0; }
        )
        ==
        Outcome<void, double>{err, 40.0}
    );

    STATIC_REQUIRE(
        mapError(
            Outcome<void, double>{ok}, 
            [](double x) { return x * 2.0; }
        )
        ==
        Outcome<void, double>{ok}
    );

    STATIC_REQUIRE(
        mapError(
            Outcome<int, void>{err}, 
            []() { return 10; }
        )
        ==
        Outcome<int, int>{err, 10}
    );

    STATIC_REQUIRE(
        mapError(
            Outcome<int, void>{ok, 10}, 
            []() { return 10; }
        )
        ==
        Outcome<int, int>{ok, 10}
    );
}

TEST_CASE("chain with Outcome should transform values into new Outcomes")
{
    STATIC_REQUIRE(
        chain(
            Outcome<double, int>{ok, 10.0},
            [](double x) { return Outcome<double, int>{ok, x*2.0}; }
        )
        ==
        Outcome<double, int>{ok, 20.0}
    );
}*/