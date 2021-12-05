#include "../catch.hpp"
#include "tmp/type_list.hpp"

using namespace tmp;

TEST_CASE("Type list")
{
    SECTION("Concat on empty TypeList should yield an empty TypeList")
    {
        using ResultType = typename concat_<TypeList<>>::template apply<TypeList>;

        STATIC_REQUIRE(std::is_same_v<ResultType, TypeList<>>);
    }

    SECTION("Concat on one TypeList should yield the same TypeList")
    {
        using ResultType = typename concat_<TypeList<int, char>>::template apply<TypeList>;

        STATIC_REQUIRE(std::is_same_v<ResultType, TypeList<int, char>>);
    }

    SECTION("Concat on two typelists should merge the lists in order")
    {
        using ResultType = typename concat_<TypeList<int, char>, TypeList<float, char, long>>::template apply<TypeList>;

        STATIC_REQUIRE(std::is_same_v<ResultType, TypeList<int, char, float, char, long>>);
    }

    SECTION("Concat on three typelists should merge the lists in order")
    {
        using ResultType = typename concat_<TypeList<int>, TypeList<float>, TypeList<char>>::template apply<TypeList>;

        STATIC_REQUIRE(std::is_same_v<ResultType, TypeList<int, float, char>>);
    }

    SECTION("Concat on ten typelists should merge the lists in order")
    { 
        using ResultType = typename concat_<
            TypeList<int>, 
            TypeList<float>, 
            TypeList<char>,
            TypeList<bool>,
            TypeList<long>,
            TypeList<int>, 
            TypeList<float>, 
            TypeList<char>,
            TypeList<bool>,
            TypeList<long>>::template apply<TypeList>;

        STATIC_REQUIRE(std::is_same_v<ResultType, TypeList<int, float, char, bool, long, int, float, char, bool, long>>);
    }

    SECTION("Unique on should merge the lists in order without duplicates")
    { 
        using ResultType = typename unique_<TypeList<int, float, char, bool, long, int, float, char, bool, long>>
            ::template apply<TypeList>;

        STATIC_REQUIRE(std::is_same_v<ResultType, TypeList<int, float, char, bool, long>>);
    }

    SECTION("Unique on should not flatten")
    { 
        using ResultType = typename unique_<TypeList<TypeList<int>, TypeList<>, float>>
            ::template apply<TypeList>;

        STATIC_REQUIRE(std::is_same_v<ResultType, TypeList<TypeList<int>, TypeList<>, float>>);
    }
}