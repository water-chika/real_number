#include <real_number.hpp>

int main() {
	water::real_number::real_number r{};
	r = r + 2;
	r = r / 3;
	r = sqrt(r);
	r = r + 2;
	r = r / 3;
	r = r * sqrt(water::real_number::real_number{ 2 });
	std::cout << r << std::endl;
	return 0;
}