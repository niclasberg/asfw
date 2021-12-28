#pragma once

namespace async
{
    namespace detail
    {
        template<class Op>
        concept Operator = requires
            {
                &Op::setValue;
                &Op::setSignal;
                &Op::setError;
            };

        template<class R, class Op>
        struct TransformingReceiver
        {
            R receiver_;
            Op operator_;

            template<class ... Values> 
            void setValue(Values && ... values) && 
            { 
                operator_.setValue(receiver_, static_cast<Values&&>(values)...);
            }

            template<class T>
            void setSignal(T && signal)
            {
                operator_.setSignal(receiver_, static_cast<Values&&>(values)...);
            }

            template<class E> 
            void setError(E && e) && 
            { 
                operator_.setError(receiver_, static_cast<E&&>(e));
            }

            void setDone() && 
            { 
                operator_.setDone(receiver_);
            }
        };

        template<class S, class Op>
        class TransformingSender
        {
        public:
            template<template<typename...> class Variant, template<template...> class Tuple>
            using value_types = typename Op::template value_types<S, Variant, Tuple>;

            template<template<typename...> class Variant>
            using signal_types = typename Op::template signal_types<S, Variant>;

            template<template<typename...> class Variant>
            using error_types = typename Op::template error_types<S, Variant>;

            template<class R>
            auto connect(R && receiver) &&
                -> ConnectResultType<S, LiftReceiver<std::remove_cvref_t<R>, F>>
            {
                return async::connect(
                    std::move(sender_), 
                    LiftReceiver<std::remove_cvref_t<R>, F>{static_cast<R&&>(receiver), std::move(liftedOperation_)});
            }

        private:
            S sender_;
            F liftedOperation_;
        };
    }
}