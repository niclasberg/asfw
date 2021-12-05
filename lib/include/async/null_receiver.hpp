#pragma once

namespace async
{
    /**
     * Receiver that does nothing, useful for testing
     */
    struct NullReceiver
    {
        template<class ... Values>
        void setValue(Values &&...) &&
        {
            
        }

        template<class ... Values> 
        void setNext(Values &&...)
        {
            
        }

        template<class E>
        void setError(E &&) &&
        {

        }

        void setDone() &&
        {

        }
    };
}