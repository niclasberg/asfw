#pragma once

namespace async
{
    namespace detail
    {
        template<class ParentSender, class R>
        class FilterOperation
        {
        public:
            template<class ... Ts>
            void setNext(Ts && ... args) &
            {

            }

            template<class E>
            void setError(E && e) &&
            {

            }

            void setDone() &&
            {

            }

            void start()
            {

            }

        private:

        };
    }
}