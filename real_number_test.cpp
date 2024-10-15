#include <real_number.hpp>

int main() {
	water::real_number::real_number r{ water::real_number::rational_number{0.0} };
	r = r + water::real_number::real_number{ water::real_number::rational_number{ 2 } };
	r = sqrt(r);
	r = r + water::real_number::real_number{ water::real_number::rational_number{2} };
	r = r / water::real_number::real_number{ water::real_number::rational_number{3} };
	std::cout << r << std::endl;
	return 0;
}