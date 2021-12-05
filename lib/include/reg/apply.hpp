#pragma once
#include "field_action.hpp"
#include "combine.hpp"

namespace reg
{
    inline constexpr struct apply_t 
    {
        template<class T, class _Location, class ... Actions>
        constexpr decltype(auto) operator()(T && handler, FieldAction<_Location, Actions> && ... fieldActions) const
        {
            auto combined = detail::makeCombinedAction(static_cast<Actions&&>(fieldActions)...);
            using Combined = decltype(combined);
            constexpr bool doesWrite = Combined::isWrite;
            constexpr bool doesRead = !std::is_same_v<
                typename Combined::template ValueTypes<detail::pack_t>,
                detail::pack_t<>>;

            if constexpr (doesRead && doesWrite)
            {
                return static_cast<T&&>(handler).readModifyWriteTransform(_Location{}, combined);
            }
            else if constexpr (doesRead && !doesWrite)
            {
                return static_cast<T&&>(handler).readAndTransform(_Location{}, combined);
            }
            else if constexpr (!doesRead && doesWrite)
            {
                return static_cast<T&&>(handler).readModifyWrite(_Location{}, combined);
            }
        }
    } apply{};
}