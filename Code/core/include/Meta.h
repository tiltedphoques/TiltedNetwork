#pragma once

#include <type_traits>

namespace details
{
    template <class Default, class AlwaysVoid,
        template<class...> class Op, class... Args>
    struct detector {
        using value_t = std::false_type;
        using type = Default;
    };

    template <class Default, template<class...> class Op, class... Args>
    struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
        using value_t = std::true_type;
        using type = Op<Args...>;
    };

    struct nonesuch {

        ~nonesuch() = delete;
        nonesuch(nonesuch const&) = delete;
        void operator=(nonesuch const&) = delete;
    };

    template <template<class...> class Op, class... Args>
    using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

    template <template<class...> class Op, class... Args>
    using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

    template< template<class...> class Op, class... Args >
    constexpr bool is_detected_v = is_detected<Op, Args...>::value;
}
