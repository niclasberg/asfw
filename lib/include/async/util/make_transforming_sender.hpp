#pragma once

namespace async::util
{
    /*template<class T>
    concept TransformingOperation = 
        requires (T & t)
        {
            t.onStart();
            {&T::onValue};
            t.onDone();

        };*/

    class Operation
    {
        template<class R>
        void onStart(R & receiver)
        {

        }

        template<class R, class ... Args>
        void onValue(R & receiver, Args && ... args)
        {

        }
    };

    namespace detail
    {
        template<class S, class O, class R>
        class TransformingOperation
        {
            class InnerReceiver;
            friend class InnerReceiver;

            class InnerReceiver
            {
            public:
                template<class ... Args>
                void setValue(Args && ... args) &&
                {
                    op_.inner_.onValue(receiver_, static_cast<Args&&>(args)...);
                }

                template<class E>
                void setError(E && error) &&
                {
                    op_.inner_.onError(receiver_, static_cast<E&&>(error));
                }

                void setDone() && 
                {
                    op_.inner_.onDone();
                }

            private:
                TransformingOperation & op_;
                R receiver_;
            };

        public:
            

        private:
            O inner_;
        };

        template<class S, class F>
        class TransformingSender
        {

        };
    }

    template<class E, class ... ValueTypes>
    struct makeTransformingSender_t 
    {
        template<class S, class F>
        auto operator()(S && parentSender, F && factory) const
            -> detail::SourceSender<std::remove_cvref_t<F>, E, ValueTypes...>
        {
            return {static_cast<F&&>(factory)};
        }
    };
}