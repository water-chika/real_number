#include "real_number.hpp"

water::real_number::sqrt_expr::sqrt_expr()
	:expr{new real_number}
{
	*expr = real_number{ rational_number{0} };
}

water::real_number::sqrt_expr::sqrt_expr(const real_number& r)
	:expr{new real_number}
{
	*expr = r;
}

water::real_number::sqrt_expr::sqrt_expr(const sqrt_expr& e)
	: expr{new real_number}
{
	*expr = *e.expr;
}

water::real_number::sqrt_expr& water::real_number::sqrt_expr::operator=(const sqrt_expr& e)
{
	*expr = *e.expr;
	return *this;
}
