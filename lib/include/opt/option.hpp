#pragma once
#include "types.hpp"
#include <boost/hana/pair.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/find.hpp>
#include <boost/hana/at_key.hpp>
#include <boost/hana/is_subset.hpp>
#include <boost/hana/contains.hpp>

namespace opt
{
    template<class K, class V>
    using Option = boost::hana::pair<K, V>;

    constexpr auto makeOption = boost::hana::make_pair;

    template<class ... Ks> 
    struct Fields { };

    template<class ... Ks>
    constexpr Fields<Ks...> fields(Ks...) { return {}; }

    template<class ... Ks, class ... Options>
    constexpr decltype(auto) makeOptionSet(Fields<Ks...>, Options && ... opts)
    {
        auto options = hana::make_map(std::forward<Options>(opts)...);
        static_assert(hana::is_subset(
                    hana::keys(decltype(options){}),
                    hana::make_basic_tuple(Ks{}...)),
                    "Invalid option supplied");
        return options;
    }

    inline constexpr struct value_t 
    {
        template<class K, class V>
        constexpr bool operator()(const Option<K, V> & option) const
        {
            return hana::first(option);
        }

        template<class K, class V>
        constexpr bool operator()(Option<K, V> && option) const
        {
            return hana::first(std::move(option));
        }
    } value{};

    inline constexpr struct contains_t 
    {
        template<class Map, class K>
        constexpr bool operator()(Map && options, K key) const
        {
            return hana::contains(static_cast<Map&&>(options), key);
        }
    } contains{};

    inline constexpr struct get_t
    {
        template<class Map, class K>
        constexpr decltype(auto) operator()(Map && options, K&& key) const
        {
            return hana::at_key(static_cast<Map&&>(options), static_cast<K&&>(key));
        }
    } get{};

    inline constexpr struct getOrDefault_t
    {
        template<class Map, class K, class V>
        constexpr auto operator()(Map && options, K key, V && defaultValue) const
        {
            return hana::find(options, key).value_or(static_cast<V&&>(defaultValue));
        }

        template<class Map, class K, class V>
        constexpr auto operator()(Map && options, K key, const Option<K, V> & defaultOption) const
        {
            return hana::find(options, key)
                .value_or(hana::second(defaultOption));
        }
    } getOrDefault{};
}

#define MAKE_OPTION_KEY(X) struct X##_t{}; constexpr auto X = hana::type_c<X##_t>