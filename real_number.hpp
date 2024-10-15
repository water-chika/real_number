#pragma once

#include <cstdint>
#include <memory>
#include <variant>
#include <iostream>
#include <cassert>

namespace water {
	namespace real_number {
		struct rational_number {
			rational_number() : rational_number{ 0 } {}
			rational_number(std::integral auto i) : x{  static_cast<double>(i) } {}
			rational_number(std::floating_point auto f) : x{ f } {}
			double x;
		};
		struct real_number;
		struct sqrt_expr{
			sqrt_expr();
			sqrt_expr(const real_number& r);
			sqrt_expr(const sqrt_expr& e);
			sqrt_expr& operator=(const sqrt_expr& e);
			~sqrt_expr() {
				delete expr;
			}
			real_number* expr;
		};
		template<char OP>
		struct binary_expr {
			binary_expr() = delete;
			binary_expr(const real_number& l, const real_number& r);
			binary_expr(const binary_expr& e);
			binary_expr& operator=(const binary_expr& e);
			~binary_expr() {
				delete left;
				delete right;
			}
			real_number* left;
			real_number* right;
		};

		struct real_number {
			real_number() : real_number{ 0.0 } {}
			real_number(std::integral auto i) : real_number{ rational_number{i} } {}
			real_number(std::floating_point auto i) : real_number{ rational_number{i} } {}
			real_number(const rational_number& q) : exp{ q } {}
			std::variant<rational_number,
				sqrt_expr,
				binary_expr<'+'>,
				binary_expr<'-'>,
				binary_expr<'*'>,
				binary_expr<'/'>> exp;
		};

		inline std::ostream& operator<<(std::ostream& out, real_number r) {
			if (std::holds_alternative<rational_number>(r.exp)) {
				out << std::get<rational_number>(r.exp).x;
			}
			else if (std::holds_alternative<sqrt_expr>(r.exp)) {
				out << "sqrt(" << *std::get<sqrt_expr>(r.exp).expr << ")";
			}
			else if (std::holds_alternative<binary_expr<'+'>>(r.exp)) {
				out << *std::get<binary_expr<'+'>>(r.exp).left << "+" << *std::get<binary_expr<'+'>>(r.exp).right;
			}
			else if (std::holds_alternative<binary_expr<'-'>>(r.exp)) {
				out << '(' << * std::get<binary_expr<'-'>>(r.exp).left << ')'
					<< "-" 
					<< '(' << * std::get<binary_expr<'-'>>(r.exp).right << ')';
			}
			else if (std::holds_alternative<binary_expr<'*'>>(r.exp)) {
				out << '(' << * std::get<binary_expr<'*'>>(r.exp).left << ')'
					<< "*"
					<< '(' << *std::get<binary_expr<'*'>>(r.exp).right << ')';
			}
			else if (std::holds_alternative<binary_expr<'/'>>(r.exp)) {
				out << '(' << * std::get<binary_expr<'/'>>(r.exp).left << ')'
					<< "/"
					<< '(' << * std::get<binary_expr<'/'>>(r.exp).right << ')';
			}
			else {
				assert(0);
			}
			return out;
		}

		inline real_number operator+(real_number lhs, real_number rhs) {
			real_number res{};
			if (std::holds_alternative<rational_number>(lhs.exp) &&
				std::holds_alternative<rational_number>(rhs.exp)) {
				res.exp = rational_number{ std::get<rational_number>(lhs.exp).x + std::get<rational_number>(rhs.exp).x };
			}
			else {
				res.exp = binary_expr<'+'>{lhs, rhs};
			}
			return res;
		}
		inline real_number operator-(real_number lhs, real_number rhs) {
			real_number res{};
			if (std::holds_alternative<rational_number>(lhs.exp) &&
				std::holds_alternative<rational_number>(rhs.exp)) {
				res.exp = rational_number{ std::get<rational_number>(lhs.exp).x - std::get<rational_number>(rhs.exp).x };
			}
			else {
				res.exp = binary_expr<'-'>{ lhs, rhs };
			}
			return res;
		}
		inline real_number operator/(real_number lhs, real_number rhs) {
			real_number res{};
			if (std::holds_alternative<rational_number>(lhs.exp) &&
				std::holds_alternative<rational_number>(rhs.exp)) {
				res.exp = rational_number{ std::get<rational_number>(lhs.exp).x / std::get<rational_number>(rhs.exp).x };
			}
			else {
				res.exp = binary_expr<'/'>{ lhs, rhs };
			}
			return res;
		}
		inline real_number operator*(real_number lhs, real_number rhs) {
			real_number res{};
			if (std::holds_alternative<rational_number>(lhs.exp) &&
				std::holds_alternative<rational_number>(rhs.exp)) {
				res.exp = rational_number{ std::get<rational_number>(lhs.exp).x * std::get<rational_number>(rhs.exp).x };
			}
			else {
				res.exp = binary_expr<'*'>{ lhs, rhs };
			}
			return res;
		}
		inline real_number sqrt(real_number r) {
			real_number res{};
			res.exp = sqrt_expr{ r };
			return res;
		}
		template<char OP>
		inline binary_expr<OP>::binary_expr(const real_number& l, const real_number& r)
			: left{new real_number}, right{new real_number}
		{
			*left = l;
			*right = r;
		}
		template<char OP>
		inline binary_expr<OP>::binary_expr(const binary_expr& e)
			: binary_expr{ *e.left, *e.right }
		{
		}
		template<char OP>
		inline binary_expr<OP>& binary_expr<OP>::operator=(const binary_expr& e)
		{
			*left = *e.left;
			*right = *e.right;
			return *this;
		}
	}
}