// Thanks @cyrmax
#pragma once

#include <concepts>
#include <functional>

template <std::invocable Func> class ScopeGuard {
  public:
    explicit ScopeGuard(Func func) noexcept : m_function(std::move(func)) {}

    ~ScopeGuard() {
        if (m_isActive) {
            m_function();
        }
    }

    void dismiss() noexcept { m_isActive = false; }

    ScopeGuard(const ScopeGuard&) = delete;
    auto operator=(const ScopeGuard&) -> ScopeGuard& = delete;
    ScopeGuard(ScopeGuard&&) = delete;
    auto operator=(ScopeGuard&&) -> ScopeGuard& = delete;

  private:
    Func m_function;
    bool m_isActive{true};
};

#define DEFER_CONCAT_IMPL_(x, y) x##y
#define DEFER_CONCAT_(x, y) DEFER_CONCAT_IMPL_(x, y)
#define defer(code) auto DEFER_CONCAT_(defer_object_, __LINE__) = ScopeGuard([&]() { code; })
