#pragma once

#include <cstdint>
#include <memory>
#include <variant>
#include <iostream>
#include <cassert>
#include <vector>

namespace real_number {
    using offset_t = size_t;
    class int64 {
    public:
        int64() = default;
        int64(int64_t v) : m_v{v} {}
        auto value() {
            return m_v;
        }
    private:
        int64_t m_v;
    };
    std::ostream& operator<<(std::ostream& out, int64 v) {
        return out << v.value();
    }
    template<char OP>
    class binary_expr {
    public:
        binary_expr() = default;
        binary_expr(offset_t arg0, offset_t arg1) : m_args{arg0, arg1} {}
        auto arg_offset(int index) {
            return m_args[index];
        }
    private:
        offset_t m_args[2];
    };
    class sqrt_expr {
    public:
        sqrt_expr() = default;
        sqrt_expr(offset_t arg) : m_arg{arg} {}
    private:
        offset_t m_arg;
    };

    class real_number {
    public:
        using expr_variant = 
            std::variant<
                int64,
                binary_expr<'+'>,
                binary_expr<'-'>,
                binary_expr<'*'>,
                binary_expr<'/'>,
                sqrt_expr
                >;
        real_number(int64_t v) : m_expr{int64{v}} {}
        real_number() : real_number(0) {}
        real_number(expr_variant expr) : m_expr{expr} {}
        template<char OP>
        real_number(binary_expr<OP> expr, real_number arg0, real_number arg1)
            : m_expr(1+arg0.m_expr.size()+arg1.m_expr.size()) {
                m_expr[0] = typeof(expr){0, arg0.m_expr.size()};
                std::ranges::copy(arg0.m_expr, m_expr.begin()+1);
                std::ranges::copy(arg1.m_expr, m_expr.begin()+1+arg0.m_expr.size());
            }
        real_number(sqrt_expr expr, real_number arg)
            : m_expr(1 + arg.m_expr.size()) {
                m_expr[0] = typeof(expr){0};
                std::ranges::copy(arg.m_expr, m_expr.begin() + 1);
            }

        auto size() const {
            return static_cast<offset_t>(m_expr.size());
        }
        auto& operator[](size_t index) const {
            return m_expr[index];
        }
        auto begin() const {
            return m_expr.begin();
        }
        auto end() const {
            return m_expr.end();
        }
    private:
        static_assert(std::is_copy_assignable_v<expr_variant>
                && std::is_copy_constructible_v<expr_variant>
                );
        std::vector<
            expr_variant,
            std::allocator<expr_variant>
            > m_expr;
    };

    real_number operator+(const real_number& lhs, const real_number& rhs) {
        return real_number{binary_expr<'+'>{}, lhs, rhs};
    }
    real_number operator-(const real_number& lhs, const real_number& rhs) {
        return real_number{binary_expr<'-'>{}, lhs, rhs};
    }
    real_number operator*(const real_number& lhs, const real_number& rhs) {
        return real_number{binary_expr<'*'>{}, lhs, rhs};
    }
    real_number operator/(const real_number& lhs, const real_number& rhs) {
        return real_number{binary_expr<'/'>{}, lhs, rhs};
    }
    real_number sqrt(const real_number& v) {
        return real_number{sqrt_expr{}, v};
    }

    // deduce

    // output
    std::ostream& output(std::ostream& out, int64 v, auto begin, auto end) {
        return out << v;
    }
    std::ostream& output(std::ostream& out, sqrt_expr expr, auto begin, auto end) {
        out << "sqrt(";
        output(out, *begin, begin+1, end);
        out << ")";
        return out;
    }
    template<char OP>
    std::ostream& output(std::ostream& out, binary_expr<OP> expr, auto begin, auto end) {
        out << "(";
        output(out, *begin, begin+1, begin + expr.arg_offset(1));
        out << ")";
        out << OP;
        out << "(";
        output (out, *(begin+expr.arg_offset(1)), begin+expr.arg_offset(1)+1, end);
        out << ")";
        return out;
    }
    std::ostream& output(std::ostream& out, real_number::expr_variant expr, auto begin, auto end) {
        return std::visit(
                [&out, &begin, &end](auto& t) -> auto& {
                    return output(out, t, begin, end);
                },
                expr
                );
    }

    std::ostream& operator<<(std::ostream& out, const real_number& v) {
        return std::visit(
                [&out, &v](auto& t) -> auto&{
                    return output(out, t, v.begin()+1, v.end());
                },
                v[0]);
    }
}
