#pragma once

#include <cstdint>
#include <memory>
#include <variant>
#include <iostream>
#include <cassert>
#include <vector>
#include <utility>
#include <map>

namespace real_number {
    using offset_t = size_t;
    class int64 {
    public:
        int64() = default;
        int64(int64_t v) : m_v{v} {}
        auto value() const {
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
        auto arg_offset(int index) const {
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
        static_assert(std::is_copy_assignable_v<expr_variant>
                && std::is_copy_constructible_v<expr_variant>
                );
        using container_type = std::vector<
            expr_variant
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
        real_number(container_type::const_iterator begin, container_type::const_iterator end)
            : m_expr{begin, end} {
        }
        real_number(expr_variant expr, container_type::const_iterator begin, container_type::const_iterator end)
            : m_expr(1 + (end - begin)){
                m_expr[0] = expr;
                std::copy(begin, end, m_expr.begin() + 1);
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
        auto& expr() const {
            return m_expr;
        }
    private:
        container_type m_expr;
    };

    template<class T, class U>
        requires (!std::same_as<T, U>)
    bool expr_variant_equal(const T& lhs, const U& rhs) {
        return false;
    }
    bool expr_variant_equal(const sqrt_expr& lhs, sqrt_expr& rhs) {
        return true;
    }
    template<char OP>
    bool expr_variant_equal(const binary_expr<OP>& lhs, binary_expr<OP>& rhs) {
        return lhs.arg_offset(0) == rhs.arg_offset(0) &&
            lhs.arg_offset(1) == rhs.arg_offset(1);
    }
    bool expr_variant_equal(const int64& lhs, const int64& rhs) {
        return lhs.value() == rhs.value();
    }
    bool expr_variant_equal(const real_number::expr_variant& lhs, const real_number::expr_variant& rhs) {
        return std::visit(
                [](auto& lhs, auto& rhs) {
                    return expr_variant_equal(lhs, rhs);
                },
                lhs, rhs);
    }
    std::ostream& operator<<(std::ostream& out, const real_number& v);

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

    real_number deduce_0_expr_mul(real_number::expr_variant expr, auto begin0, auto end0,
            auto begin1, auto end1) {
        if (std::holds_alternative<binary_expr<'+'>>(expr)) {
            auto add_expr = std::get<binary_expr<'+'>>(expr);
            auto muled = real_number{begin1, end1};
            auto added0 = real_number{begin0, begin0+add_expr.arg_offset(1)};
            auto added1 = real_number{begin0+add_expr.arg_offset(1), end0};
            auto res = added0 * muled + added1 * muled;
            return res;
        }
        else {
            auto res = real_number{expr, begin0, end0} * real_number{begin1, end1};
            return res;
        }
    }
    template<char OP>
    real_number deduce_0_expr(auto expr, auto begin, auto end) {
        return expr;
    }
    real_number deduce_0_expr(binary_expr<'*'> expr, auto begin, auto end) {
        return deduce_0_expr_mul(*begin, begin+1, begin+expr.arg_offset(1),
                begin+expr.arg_offset(1), end);
    }
    real_number deduce_0_expr(real_number::expr_variant expr, auto begin, auto end) {
        return std::visit(
                [&begin, &end] (auto& t) {
                    return deduce_0_expr(t, begin, end);
                },
                expr
                );
    }
    real_number deduce_0(const real_number& v) {
        return deduce_0_expr(v[0], v.begin()+1, v.end());
    }

    real_number deduce_1_expr_mul(real_number::expr_variant expr, auto begin0, auto end0,
            auto begin1, auto end1) {
        if (std::holds_alternative<sqrt_expr>(expr) &&
                std::holds_alternative<sqrt_expr>(*begin1) &&
                std::equal(begin0, end0, begin1+1, end1,
                    [](auto& lhs, auto& rhs) {
                        return expr_variant_equal(lhs, rhs);
                        })
                ) {
            return real_number{begin0, end0};
        }
        else {
            auto res = real_number{expr, begin0, end0} * real_number{begin1, end1};
            return res;
        }
    }
    real_number deduce_1_expr(auto expr, auto begin, auto end) {
        return real_number{expr, begin, end};
    }
    template<char OP>
    real_number deduce_1_expr(binary_expr<OP> expr, auto begin, auto end) {
        return real_number{expr,
            deduce_1_expr_variant(*begin, begin+1, begin+expr.arg_offset(1)),
            deduce_1_expr_variant(
                    *(begin+expr.arg_offset(1)),
                    begin+expr.arg_offset(1)+1,
                    end)
        };
    }
    real_number deduce_1_expr(binary_expr<'*'> expr, auto begin, auto end) {
        return deduce_1_expr_mul(*begin, begin+1, begin+expr.arg_offset(1),
                begin+expr.arg_offset(1), end);
    }
    real_number deduce_1_expr_variant(real_number::expr_variant expr, auto begin, auto end) {
        return std::visit(
                [&begin, &end] (auto& t) {
                    return deduce_1_expr(t, begin, end);
                },
                expr
                );
    }

    real_number deduce_1(const real_number& v) {
        return deduce_1_expr_variant(v[0], v.begin()+1, v.end());
    }

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

    void left_deep_visit_expr(int64 v, auto begin, auto end, auto& visitor) {
        visitor(v);
    }
    void left_deep_visit_expr(auto expr, auto begin, auto end, auto& visitor) {
        visitor(expr);
        left_deep_visit_expr_variant(*begin, begin+1, end, visitor);
    }
    template<char OP>
    void left_deep_visit_expr(binary_expr<OP> expr, auto begin, auto end, auto& visitor) {
        visitor(expr);
        left_deep_visit_expr_variant(*begin, begin+1, begin + expr.arg_offset(1), visitor);
        left_deep_visit_expr_variant(*(begin+expr.arg_offset(1)), begin+expr.arg_offset(1)+1, end, visitor);
    }

    void left_deep_visit_expr_variant(real_number::expr_variant expr, auto begin, auto end, auto& visitor) {
        std::visit(
                [&begin, &end, &visitor](auto& t) {
                    left_deep_visit_expr(t, begin, end, visitor);
                },
                expr);
    }

    void left_deep_visit(const real_number& v, auto& visitor) {
        left_deep_visit_expr_variant(v[0], v.begin()+1, v.end(), visitor);
    }


    std::ostream& operator<<(std::ostream& out, const real_number& v) {
        std::vector<std::tuple<char, int,int>> priv_ranges{};
        int begin = 0, end = v.size();
        int prev_prio = 0;
        do{
            int count = 0;
            for (auto i = begin; i < end; i++) {
                auto& expr = v[i];
                if (std::holds_alternative<int64>(expr)) {
                    auto& int64_expr = std::get<int64>(expr);
                    out << int64_expr;
                }
                else if (std::holds_alternative<binary_expr<'+'>>(expr)) {
                    auto& b_e = std::get<binary_expr<'+'>>(expr);
                    if (prev_prio > 0) {
                        prev_prio = 0;
                        out << "(";
                        priv_ranges.emplace_back(')', 0, 0);
                    }
                    priv_ranges.emplace_back('+', i +1+ b_e.arg_offset(1), end);
                    end = i +1+ b_e.arg_offset(1);
                }
                else if (std::holds_alternative<binary_expr<'-'>>(expr)) {
                    auto& b_e = std::get<binary_expr<'-'>>(expr);
                    if (prev_prio > 0) {
                        prev_prio = 0;
                        out << "(";
                        priv_ranges.emplace_back(')', 0, 0);
                    }
                    priv_ranges.emplace_back('-', i+1+b_e.arg_offset(1), end);
                    end = i+1+b_e.arg_offset(1);
                }
                else if (std::holds_alternative<binary_expr<'*'>>(expr)) {
                    auto& b_e = std::get<binary_expr<'*'>>(expr);
                    priv_ranges.emplace_back('*', i+1+b_e.arg_offset(1), end);
                    end = i+1+b_e.arg_offset(1);
                    prev_prio = 1;
                }
                else if (std::holds_alternative<binary_expr<'/'>>(expr)) {
                    auto& b_e = std::get<binary_expr<'/'>>(expr);
                    priv_ranges.emplace_back('/', i+1+b_e.arg_offset(1), end);
                    end = i+1+b_e.arg_offset(1);
                    prev_prio = 1;
                }
                else if (std::holds_alternative<sqrt_expr>(expr)) {
                    out << "sqrt(";
                    count++;
                }
            }
            for (int i = 0; i < count; i++) {
                out << ")";
            }

            if (!priv_ranges.empty()) {
                auto [op, prev_begin, prev_end] = priv_ranges.back();
                priv_ranges.pop_back();
                out << op;
                begin = prev_begin;
                end = prev_end;
            }
            else {
                break;
            }
        } while (true);
        return out;
    }
}
