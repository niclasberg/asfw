#pragma once
#include <type_traits>
#include <concepts>
#include <memory>

namespace async
{
    struct success_t {};
    constexpr success_t success{};

    struct err_t {};
    constexpr err_t err{};

    struct done_t {};
    constexpr done_t done{};

    namespace detail
    {
        enum class OutcomeState
        {
            Success,
            Error, 
            Done
        };

        class OutcomeBase
        {
        public:
            constexpr OutcomeBase(success_t) : state_{OutcomeState::Success} {}
            constexpr OutcomeBase(err_t) : state_{OutcomeState::Error} { }
            constexpr OutcomeBase(done_t) : state_{OutcomeState::Done} { }

            constexpr OutcomeBase(const OutcomeBase &) = default;
            constexpr OutcomeBase(OutcomeBase &&) = default;
            constexpr ~OutcomeBase() = default;
            constexpr OutcomeBase & operator=(const OutcomeBase &) = default;
            constexpr OutcomeBase & operator=(OutcomeBase &&) = default;

            constexpr bool isSuccess() const { return this->state_ == detail::OutcomeState::Success; }
            constexpr bool isError() const { return this->state_ == detail::OutcomeState::Error; }
            constexpr bool isDone() const { return this->state_ == detail::OutcomeState::Done; }

        protected:
            OutcomeState state_;
        };

        template<class T, class E, 
            bool valueIsTriviallyDestructible = std::is_trivially_destructible_v<T>, 
            bool errorIsTriviallyDestructible = std::is_trivially_destructible_v<E>>
        class OutcomeStorage : public OutcomeBase
        {
        public:
            template<class ... Args>
            constexpr OutcomeStorage(success_t, Args && ... args) 
                : OutcomeBase{success}, value_{static_cast<Args&&>(args)...} {}

            template<class ... Args>
            constexpr OutcomeStorage(err_t, Args && ... args) 
                : OutcomeBase{err}, error_{static_cast<Args&&>(args)...} { }

            constexpr OutcomeStorage(done_t) : OutcomeBase(done) { }

            constexpr OutcomeStorage(const OutcomeStorage & rhs) 
                requires std::copy_constructible<T> && std::copy_constructible<E>
                : OutcomeBase(rhs)
            {
                assign_from(rhs);
            }

            constexpr OutcomeStorage(OutcomeStorage && rhs) 
                requires std::move_constructible<T> && std::move_constructible<E>
                : OutcomeBase(rhs)
            {
                assign_from(static_cast<OutcomeStorage&&>(rhs));
            }

            constexpr OutcomeStorage & operator=(const OutcomeStorage & rhs)
                requires std::copy_constructible<T> && std::copy_constructible<E>
            {
                if (this != &rhs)
                {
                    destroy();
                    assign_from(rhs);
                    state_ = rhs.state_;
                }

                return *this;
            }

            constexpr OutcomeStorage & operator=(OutcomeStorage && rhs)
                requires std::move_constructible<T> && std::move_constructible<E>
            {
                if (this != &rhs)
                {
                    destroy();
                    assign_from(static_cast<OutcomeStorage&&>(rhs));
                    state_ = rhs.state_;
                }

                return *this;
            }

            constexpr ~OutcomeStorage()
            {
                destroy();
            }

            constexpr T & value() & { return value_; }
            constexpr const T & value() const & { return value_; }
            constexpr T && value() && { return std::move(*this).value_; }

            constexpr E & error() & { return error_; }
            constexpr const E & error() const & { return error_; }
            constexpr E && error() && { return std::move(*this).error_; }
        
        private:
            constexpr void destroy()
            {
                if constexpr (!valueIsTriviallyDestructible)
                {
                    if (isSuccess()) value_.~T();
                }
                
                if constexpr (!errorIsTriviallyDestructible)
                {
                    if (isError()) error_.~E();
                }
            }

            template<class O>
            constexpr void assign_from(O && rhs)
            {
                if (rhs.isSuccess())
                    std::construct_at(std::addressof(value_), static_cast<O&&>(rhs).value_);
                else if (rhs.isError())
                    std::construct_at(std::addressof(error_), static_cast<O&&>(rhs).error_);
            }

            union {
                T value_;
                E error_;
            };
        };

        // T and E are both trivially destructible
        template<class T, class E>
        class OutcomeStorage<T, E, true, true> : public OutcomeBase
        {
        public:
            template<class ... Args>
            constexpr OutcomeStorage(success_t, Args && ... args) 
                : OutcomeBase{success}, value_{static_cast<Args&&>(args)...} {}

            template<class ... Args>
            constexpr OutcomeStorage(err_t, Args && ... args) 
                : OutcomeBase{err}, error_{static_cast<Args&&>(args)...} { }

            constexpr OutcomeStorage(done_t) : OutcomeBase{done} { }

            constexpr OutcomeStorage(const OutcomeStorage & rhs) 
                requires std::copy_constructible<T> && std::copy_constructible<E>
            : OutcomeBase{rhs}
            {
                assign_from(rhs);
            }

            constexpr OutcomeStorage(OutcomeStorage && rhs) 
                requires std::move_constructible<T> && std::move_constructible<E>
            : OutcomeBase{rhs}
            {
                assign_from(static_cast<OutcomeStorage&&>(rhs));
            }

            constexpr ~OutcomeStorage() = default;

            constexpr OutcomeStorage & operator=(const OutcomeStorage & rhs)
                requires std::copy_constructible<T> && std::copy_constructible<E>
            {
                if (this != &rhs)
                {
                    assign_from(rhs);
                    state_ = rhs.state_;
                }
                    
                return *this;
            }

            OutcomeStorage & operator=(OutcomeStorage && rhs)
                requires std::move_constructible<T> && std::move_constructible<E>
            {
                if (this != &rhs)
                {
                    assign_from(static_cast<OutcomeStorage&&>(rhs));
                    state_ = rhs.state_;
                }

                return *this;
            }

            constexpr T & value() & { return value_; }
            constexpr const T & value() const & { return value_; }
            constexpr T && value() && { return std::move(*this).value_; }

            constexpr E & error() & { return error_; }
            constexpr const E & error() const & { return error_; }
            constexpr E && error() && { return std::move(*this).error_; }
        
        private:
            template<class O>
            constexpr void assign_from(O && rhs)
            {
                if (rhs.isSuccess())
                    std::construct_at(std::addressof(value_), static_cast<O&&>(rhs).value_);
                else if (rhs.isError())
                    std::construct_at(std::addressof(error_), static_cast<O&&>(rhs).error_);
            }

            union {
                T value_;
                E error_;
            };
        };

        // T is void, E is trivially destructible
        template<class E>
        class OutcomeStorage<void, E, false, true> : public OutcomeBase
        {
        public:
            constexpr OutcomeStorage(success_t) : OutcomeBase{success} { }

            template<class ... Args>
            constexpr OutcomeStorage(err_t, Args && ... args) 
                : OutcomeBase(err), error_{static_cast<Args&&>(args)...} { }

            constexpr OutcomeStorage(done_t) : OutcomeBase{done} { }

            constexpr OutcomeStorage(const OutcomeStorage & rhs) : OutcomeBase(rhs)
            {
                if (rhs.state_ == OutcomeState::Error)
                    std::construct_at(std::addressof(error_), rhs.error_);
            }

            constexpr OutcomeStorage(OutcomeStorage && rhs) 
                : state_(rhs.state_)
            {
                if (rhs.state_ == OutcomeState::Error)
                    std::construct_at(std::addressof(error_), std::move(rhs.error_));
            }

            constexpr OutcomeStorage & operator=(const OutcomeStorage & rhs)
            {
                if (this != &rhs)
                {
                    state_ = rhs.state_;
                    if (rhs.state_ == OutcomeState::Error)
                        error_ = rhs.error_;
                }

                return *this;
            }

            constexpr OutcomeStorage & operator=(OutcomeStorage && rhs)
            {
                if (this != &rhs)
                {
                    state_ = rhs.state_;
                    if (rhs.state_ == OutcomeState::Error)
                        error_ = std::move(rhs.error_);
                }

                return *this;
            }

            constexpr ~OutcomeStorage() = default;

            constexpr E & error() & { return error_; }
            constexpr const E & error() const & { return error_; }
            constexpr E && error() && { return std::move(*this).error_; }

        private:
            union {
                E error_;
            };
        };

        // T is void, E is not trivially destructible
        template<class E>
        class OutcomeStorage<void, E, false, false> : OutcomeBase
        {
        public:
            constexpr OutcomeStorage(success_t) : OutcomeBase{success} { }

            template<class ... Args>
            constexpr OutcomeStorage(err_t, Args && ... args) 
                : OutcomeBase{err}, error_{static_cast<Args&&>(args)...} { }

            constexpr OutcomeStorage(done_t) : OutcomeBase{done} { }

            constexpr OutcomeStorage(const OutcomeStorage & rhs) : OutcomeBase(rhs)
            {
                assign_from(rhs);
            }

            constexpr OutcomeStorage(OutcomeStorage && rhs) : OutcomeBase(rhs)
            {
                assign_from(static_cast<OutcomeStorage&&>(rhs));
            }

            constexpr OutcomeStorage & operator=(const OutcomeStorage & rhs)
            {
                if (this != &rhs)
                {
                    destroy();
                    assign_from(rhs);
                    state_ = rhs.state_;
                }

                return *this;
            }

            constexpr OutcomeStorage & operator=(OutcomeStorage && rhs)
            {
                if (this != &rhs)
                {
                    destroy();
                    assign_from(static_cast<OutcomeStorage&&>(rhs));
                    state_ = rhs.state_;
                }

                return *this;
            }

            constexpr ~OutcomeStorage() { destroy(); }

            constexpr E & error() & { return error_; }
            constexpr const E & error() const & { return error_; }
            constexpr E && error() && { return std::move(*this).error_; }

        private: 
            constexpr void destroy()
            {
                if (isError()) error_.~E();
            }

            template<class O>
            constexpr void assign_from(O && rhs)
            {
                if (rhs.isError())
                    std::construct_at(std::addressof(error_), static_cast<O&&>(rhs).error_);
            }

            union {
                E error_;
            };
        };

        // T is trivially destructible, E is void
        template<class T>
        class OutcomeStorage<T, void, true, false> : public OutcomeBase
        {
        public:
            template<class ... Args>
            constexpr OutcomeStorage(success_t, Args && ... args) 
                : OutcomeBase(success), value_{static_cast<Args&&>(args)...} {}

            constexpr OutcomeStorage(err_t) : OutcomeBase(err) { }
            constexpr OutcomeStorage(done_t) : OutcomeBase(done) { }

            constexpr OutcomeStorage(const OutcomeStorage & rhs) : OutcomeBase(rhs)
            {
                assign_from(rhs);
            }

            constexpr OutcomeStorage(OutcomeStorage && rhs) : OutcomeBase(rhs)
            {
                assign_from(std::move(rhs));
            }

            constexpr OutcomeStorage & operator=(const OutcomeStorage & rhs)
            {
                if (this != &rhs)
                {
                    assign_from(rhs);
                    state_ = rhs.hasValue_;
                }

                return *this;
            }

            constexpr OutcomeStorage & operator=(OutcomeStorage && rhs)
            {
                if (this != &rhs)
                {
                    assign_from(static_cast<OutcomeStorage &&>(rhs));
                    state_ = rhs.state_;
                }

                return *this;
            }

            constexpr ~OutcomeStorage() = default;

            constexpr T & value() & { return value_; }
            constexpr const T & value() const & { return value_; }
            constexpr T && value() && { return std::move(*this).value_; }
        
        private:
            template<class O>
            constexpr void assign_from(O && rhs)
            {
                if (rhs.isSuccess())
                    std::construct_at(std::addressof(value_), std::move(rhs.value_));
            }

            union {
                T value_;
            };
        };

        // T is not trivially destructible, E is void
        template<class T>
        class OutcomeStorage<T, void, false, false> : public OutcomeBase
        {
        public:
            template<class ... Args>
            constexpr OutcomeStorage(success_t, Args && ... args) 
                : OutcomeBase{success}, value_{static_cast<Args&&>(args)...} {}

            constexpr OutcomeStorage(err_t) : OutcomeBase(err) { }
            constexpr OutcomeStorage(done_t) : OutcomeBase(done) { }

            constexpr OutcomeStorage(const OutcomeStorage & rhs) : OutcomeBase(rhs)
            {
                if (rhs.isSuccess())
                    std::construct_at(std::addressof(value_), std::move(rhs.value_));
            }

            constexpr OutcomeStorage(OutcomeStorage && rhs) : OutcomeBase(rhs)
            {
                if (rhs.isSuccess())
                    std::construct_at(std::addressof(value_), std::move(rhs.value_));
            }

            constexpr OutcomeStorage & operator=(const OutcomeStorage & rhs)
            {
                if (this != &rhs)
                {
                    destroy();
                    assign_from(rhs);
                    state_ = rhs.state_;
                }

                return *this;
            }

            constexpr OutcomeStorage & operator=(OutcomeStorage && rhs)
            {
                if (this != &rhs)
                {
                    destroy();
                    assign_from(std::move(rhs));
                    state_ = rhs.state_;
                }

                return *this;
            }

            constexpr ~OutcomeStorage() { destroy(); }

            constexpr T & value() & { return value_; }
            constexpr const T & value() const & { return value_; }
            constexpr T && value() && { return std::move(*this).value_; }

        private:
            constexpr void destroy()
            {
                if (isSuccess()) value_.~T();
            }

            template<class O>
            constexpr void assign_from(O && rhs)
            {
                if (rhs.isSuccess())
                    std::construct_at(std::addressof(value_), std::move(rhs.value_));
            }

            union {
                T value_;
            };
        };

        template<>
        class OutcomeStorage<void, void, false, false> : public OutcomeBase
        {
        public:
            constexpr OutcomeStorage(success_t) : OutcomeBase(success) { }
            constexpr OutcomeStorage(err_t) : OutcomeBase(err) { }
            constexpr OutcomeStorage(done_t) : OutcomeBase(done) { }

            constexpr OutcomeStorage(const OutcomeStorage&) = default;
            constexpr OutcomeStorage(OutcomeStorage&&) = default;
            constexpr ~OutcomeStorage() = default;

            constexpr OutcomeStorage & operator=(const OutcomeStorage &) = default;
            constexpr OutcomeStorage & operator=(OutcomeStorage &&) = default;
        };
    }

    template<class T, class E>
    class Outcome final : public detail::OutcomeStorage<T, E>
    {
        using Base = detail::OutcomeStorage<T, E>;
        using Self = Outcome<T, E>;

    public:
        using ValueType = T;
        using ErrorType = E;

        constexpr Outcome(const Outcome & rhs) = default;
        constexpr Outcome(Outcome && rhs) = default;
        constexpr ~Outcome() = default;
        constexpr Outcome & operator=(const Outcome &) = default;
        constexpr Outcome & operator=(Outcome &&) = default;

        template<class ... Args>
        constexpr Outcome(success_t, Args && ... args) : Base(success, static_cast<Args&&>(args)...) {}

        template<class ... Args>
        constexpr Outcome(err_t, Args && ... args) : Base(err, static_cast<Args&&>(args)...) { }

        constexpr Outcome(done_t) : Base(done) { }

        /*template<class F, class T2 = std::remove_cvref_t<tmp::CallableResultType<F, T>>>
        constexpr auto map(F && f) const -> Outcome<T2, E>
        {
            if (hasValue())
            {
                if constexpr (std::is_void_v<T>)
                {
                    if constexpr (std::is_void_v<T2>)
                    {
                        static_cast<F&&>(f)();
                        return { success };
                    }
                    else
                    {
                        return { success, static_cast<F&&>(f)() };
                    }
                }
                else 
                {
                    if constexpr (std::is_void_v<T2>)
                    {
                        static_cast<F&&>(f)(static_cast<R&&>(input).value());
                        return { ok };
                    }
                    else
                    {
                        return { ok, static_cast<F&&>(f)(static_cast<R&&>(input).value()) };
                    }
                }
            }
            else
            {
                if constexpr (std::is_void_v<E>)
                    return { err };
                else
                    return { err, static_cast<R&&>(input).error() };
            }
        }*/

    private:
        friend constexpr bool operator==(const Outcome & lhs, const Outcome & rhs) 
        {
            if (lhs.state_ != rhs.state_)
                return false;

            if constexpr (std::is_void_v<T> && std::is_void_v<E>)
            {
                return true;
            }
            else if constexpr (std::is_void_v<T>)
            {
                return lhs.isError() ? lhs.error() == rhs.error() : true;
            }
            else if constexpr (std::is_void_v<E>)
            {
                return lhs.isSuccess() ? lhs.value() == rhs.value() : true;
            }
            else
            {
                if (lhs.isSuccess())
                    return lhs.value() == rhs.value();
                else if (lhs.isError())
                    return lhs.error() == rhs.error();
                else
                    return true; // Done
            }
        }

        /*

        template<class R, class F, class E2 = std::remove_cvref_t<tmp::CallableResultType<F, E>>>
            requires (std::same_as<std::remove_cvref_t<R>, Self>)
        friend constexpr auto tag_invoke(mapError_t, R && input, F && f)
            -> Result<T, E2>
        {
            if (input.hasValue())
            {
                if constexpr (std::is_void_v<T>)
                    return { ok };
                else
                    return { ok, static_cast<R&&>(input).value() };
            }
            else
            {
                if constexpr (std::is_void_v<E>)
                {
                    if constexpr (std::is_void_v<E2>)
                    {
                        static_cast<F&&>(f)();
                        return { err };
                    }
                    else
                    {
                        return { err, static_cast<F&&>(f)() };
                    }
                }
                else 
                {
                    if constexpr (std::is_void_v<E2>)
                    {
                        static_cast<F&&>(f)(static_cast<R&&>(input).error());
                        return { err };
                    }
                    else
                    {
                        return { err, static_cast<F&&>(f)(static_cast<R&&>(input).error()) };
                    }
                }
            }
        }

        template<class R, class F, class Ret = std::remove_cvref_t<tmp::CallableResultType<F, T>>>
            requires (
                std::same_as<std::remove_cvref_t<R>, Self> && 
                detail::isResult<Ret>
            )
        friend constexpr auto tag_invoke(chain_t, R && input, F && f) -> Ret
        {
            if (input.hasValue())
            {
                if constexpr (std::is_void_v<T>)
                    return static_cast<F&&>(f)();
                else
                    return static_cast<F&&>(f)(static_cast<R&&>(input).value());
            }
            else
            {
                if constexpr (std::is_void_v<E>)
                    return { err };
                else
                    return { err, static_cast<R&&>(input).error() };
            }
        }

        template<class R, class F, class Ret = std::remove_cvref_t<tmp::CallableResultType<F, E>>>
            requires (
                std::same_as<std::remove_cvref_t<R>, Self> && 
                detail::isResult<Ret>
            )
        friend constexpr auto tag_invoke(chainError_t, R && input, F && f) -> Ret
        {
            if (input.hasValue())
            {
                if constexpr (std::is_void_v<T>)
                    return { ok };
                else
                    return { ok, static_cast<R&&>(input).value() };
            }
            else
            {
                if constexpr (std::is_void_v<E>)
                    return static_cast<F&&>(f)();
                else
                    return static_cast<F&&>(f)(static_cast<R&&>(input).error());
            }
        }

        /*template<class R>
            requires(std::same_as<std::remove_cvref_t<R>, Self>)
        friend constexpr auto tag_invoke(toOption_t, R && result) -> Option<T>
        {
            if (result.hasValue())
            {
                if constexpr (std::is_void_v<T>)
                    return { some };
                else
                    return {some, static_cast<R&&>(result).value() };
            }        
            else
            {
                return { none};
            }
        }*/

    };

    namespace detail
    {
        template<class E>
        struct makeSuccess_t final 
        {
            template<class T>
            constexpr auto operator()(T && value) const -> Outcome<std::remove_cvref_t<T>, E>
            {
                return { success, static_cast<T&&>(value) };
            }

            constexpr auto operator()() const -> Outcome<void, E>
            {
                return { success };
            }
        };

        template<class T>
        struct makeError_t final 
        {
            template<class E>
            constexpr auto operator()(E && error) const -> Outcome<T, std::remove_cvref_t<E>>
            {
                return { err, static_cast<E&&>(error) };
            }

            constexpr auto operator()() const -> Outcome<T, void>
            {
                return { err };
            }
        };
    }

    template<class T>
    inline constexpr detail::makeSuccess_t<T> makeSuccess{};

    template<class E>
    inline constexpr detail::makeError_t<E> makeError{};

    template<class T, class E>
    inline constexpr Outcome<T, E> makeDone()
    {
        return {done};
    }
}