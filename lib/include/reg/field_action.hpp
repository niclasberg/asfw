#pragma once

namespace reg
{
    template<class _Location, class _Action>
    struct FieldAction : _Action
    {
        using Value = typename _Location::Value;

        constexpr FieldAction(const _Action & action) : _Action(action) { }
        constexpr FieldAction(const FieldAction &) = default;
        constexpr FieldAction(FieldAction &&) = default;
    };

    namespace detail 
    {
        template<class _Location, class _Action>
        constexpr auto makeFieldAction(_Location, _Action && action)
        {
            return FieldAction<_Location, std::decay_t<_Action>>(std::forward<_Action>(action));
        }
    }
}