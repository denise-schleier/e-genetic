#ifndef GA_FUNCTIONAL_HPP
#define GA_FUNCTIONAL_HPP

#include <array>
#include <vector>
#include <cmath>
#include <utility>
#include <limits>
#include <type_traits>
#include <concepts>
#include <cassert>

namespace genetic_algorithm::detail
{
    template<typename T>
    constexpr auto lforward(std::remove_reference_t<T>& t) noexcept
    {
        return std::ref<std::remove_reference_t<T>>(t);
    }

    template<typename T>
    requires(!std::is_lvalue_reference_v<T>)
    constexpr T&& lforward(std::remove_reference_t<T>&& t) noexcept
    {
        return static_cast<T&&>(t);
    }

    template<typename F>
    constexpr auto compose(F&& f) noexcept
    {
        return [f = lforward<F>(f)] <typename... Args>
        (Args&&... args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
        requires std::invocable<F, Args...>
        {
            return std::invoke(f, std::forward<Args>(args)...);
        };
    }

    template<typename F, typename... Fs>
    constexpr auto compose(F&& f, Fs&&... fs) noexcept
    {
        return [f = lforward<F>(f), ...fs = lforward<Fs>(fs)] <typename... Args>
        (Args&&... args) requires std::invocable<F, Args...>
        {
            return compose(fs...)(std::invoke(f, std::forward<Args>(args)...));
        };
    }

    template<typename ValueType, std::invocable<ValueType> F>
    auto map(const std::vector<ValueType>& cont, F&& f)
    {
        using MappedType = std::invoke_result_t<std::remove_reference_t<F>, ValueType>;
        using ResultType = std::vector<std::decay_t<MappedType>>;

        ResultType result;
        result.reserve(cont.size());
        for (const auto& elem : cont)
        {
            result.push_back(std::invoke(f, elem));
        }

        return result;
    }

    template<typename ValueType, std::invocable<ValueType> F>
    auto map(const std::vector<ValueType>& cont, F&& f) requires std::is_scalar_v<ValueType>
    {
        using MappedType = std::invoke_result_t<std::remove_reference_t<F>, ValueType>;
        using ResultType = std::vector<std::decay_t<MappedType>>;

        ResultType result(cont.size());
        for (size_t i = 0; i < cont.size(); i++)
        {
            result[i] = std::invoke(f, cont[i]);
        }

        return result;
    }

    template<typename ValueType, size_t N, typename F>
    requires std::invocable<std::remove_reference_t<F>, ValueType>
    constexpr auto map(const std::array<ValueType, N>& cont, F&& f)
    {
        using MappedType = std::invoke_result_t<std::remove_reference_t<F>, ValueType>;
        using ResultType = std::array<std::decay_t<MappedType>, N>;

        ResultType result;
        for (size_t i = 0; i < N; i++)
        {
            result[i] = std::invoke(f, cont[i]);
        }
        return result;
    }

    template<typename T>
    std::vector<T> flatten(const std::vector<std::pair<T, T>>& pairs)
    {
        assert(pairs.size() <= (std::numeric_limits<size_t>::max() / 2));

        std::vector<T> flat;
        flat.reserve(2 * pairs.size());

        for (size_t i = 0; i < pairs.size(); i++)
        {
            flat.push_back(pairs[i].first);
            flat.push_back(pairs[i].second);
        }

        return flat;
    }

    template<typename T>
    std::vector<T> flatten(std::vector<std::pair<T, T>>&& pairs)
    {
        assert(pairs.size() <= (std::numeric_limits<size_t>::max() / 2));

        std::vector<T> flat;
        flat.reserve(2 * pairs.size());

        for (size_t i = 0; i < pairs.size(); i++)
        {
            flat.push_back(std::move_if_noexcept(pairs[i].first));
            flat.push_back(std::move_if_noexcept(pairs[i].second));
        }

        return flat;
    }

    template<typename T>
    constexpr auto multiply_by(const T& multiplier)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return [=](const T& value) noexcept(noexcept(value * value))
        {
            return multiplier * value;
        };
    }

    template<typename T>
    constexpr auto divide_by(const T& divisor)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return [idiv = 1.0 / divisor](const T& value) noexcept(noexcept(value * value))
        {
            return idiv * value;
        };
    }

    template<typename T>
    constexpr auto add(const T& increment)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return [=](const T& value) noexcept(noexcept(value + value))
        {
            return value + increment;
        };
    }

    template<typename T>
    constexpr auto subtract(const T& decrement)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return [=](const T& value) noexcept(noexcept(value - value))
        {
            return value - decrement;
        };
    }

    template<typename T>
    constexpr auto multiply_add(const T& multiplier, const T& increment)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return [=](const T& value) noexcept(noexcept(value * value - value))
        {
            return multiplier * value + increment;
        };
    }

    template<typename T>
    requires (std::is_arithmetic_v<T>)
    constexpr auto multiply_add(const T& multiplier, const T& increment) noexcept
    {
        return [=](const T& value) noexcept
        {
            return std::fma(multiplier, value, increment);
        };
    }

    template<typename T>
    constexpr auto equal_to(const T& rhs)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return [=](const T& lhs) noexcept(noexcept(lhs == lhs))
        {
            return lhs == rhs;
        };
    }

    template<typename T>
    constexpr auto not_equal_to(const T& rhs)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return [=](const T& lhs) noexcept(noexcept(lhs != lhs))
        {
            return lhs != rhs;
        };
    }
    
} // namespace genetic_algorithm::detail

#endif // !GA_FUNCTIONAL_HPP