#include "../catch.hpp"
#include "cont/box_union.hpp"

TEST_CASE("Box union")
{
    SECTION("Construct")
    {
        cont::BoxUnion<int, char> boxUnion;

        boxUnion.construct(cont::union_t<int>, 5);
        
    }
}