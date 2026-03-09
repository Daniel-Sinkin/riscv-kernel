
// common.hpp

#pragma once

#include <concepts>

template <typename F>
    requires std::invocable<F&>
struct ScopeExit
{
    explicit ScopeExit(F f) : on_exit_(f)
    {
    }

    ScopeExit(const ScopeExit&) = delete;
    ScopeExit& operator=(const ScopeExit&) = delete;
    ScopeExit(ScopeExit&&) = delete;
    ScopeExit& operator=(ScopeExit&&) = delete;

    ~ScopeExit()
    {
        on_exit_();
    }

  private:
    F on_exit_;
};
