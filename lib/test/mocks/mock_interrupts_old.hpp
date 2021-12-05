#pragma once
#include <map>
#include "types.hpp"
#include <async/receiver.hpp>

class MockInterrupts
{
    template<class Receiver, std::uint8_t irqNo>
    friend class InterruptOperation;

    class InterruptOperationBase
    {
        using Callback = void (*)(InterruptOperationBase *);
        Callback onTrigger_;

    public:
        InterruptOperationBase(Callback cb) : onTrigger_(cb) { }

        void trigger()
        {
            onTrigger_(this);
        }
    }; 

    std::map<std::uint8_t, InterruptOperationBase *> irqHandlers_ = {};

    template<class Receiver, std::uint8_t irqNo>
    class InterruptOperation : public InterruptOperationBase
    {
        MockInterrupts * parent_;
        Receiver receiver_;

        static void nextImpl(InterruptOperationBase * op)
        {
            auto self = static_cast<InterruptOperation<Receiver, irqNo> *>(op);
            async::setNext(self->receiver_);
        }

    public:
        InterruptOperation(MockInterrupts * parent, Receiver && receiver) 
        : InterruptOperationBase(&nextImpl)
        , parent_(parent)
        , receiver_(std::forward<Receiver>(receiver))
        {

        }

        void start()
        {
            parent_->irqHandlers_[irqNo] = this;
        }
    };

    template<std::uint8_t irqNo>
    struct InterruptSource
    {
        MockInterrupts * parent_;

        template<template<typename...> typename Variant, template<typename...> typename Tuple> 
        using value_types = Variant<Tuple<>>;

        template<class Receiver>
        auto operator()(Receiver && receiver) const &&
        {
            return InterruptOperation<std::decay_t<Receiver>, irqNo>(parent_, std::forward<Receiver>(receiver));
        }
    }; 

public:
    template<std::uint8_t irqNo>
    InterruptSource<irqNo> getInterruptSource(Interrupt<irqNo>)
    {
        return {this};
    }

    void reset()
    {
        irqHandlers_.clear();
    }

    template<std::uint8_t irqNo>
    void triggerInterrupt(Interrupt<irqNo>)
    {
        irqHandlers_.at(irqNo)->trigger();
    }

    template<std::uint8_t irqNo>
    bool hasInterruptHandler(Interrupt<irqNo>)
    {
        return irqHandlers_.count(irqNo) != 0;
    }
};