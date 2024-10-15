#include <real_number.hpp>

int main() {
	water::real_number::real_number r{};
	r = r + 2;
	r = sqrt(r);
	r = r + 2;
	r = r / 3;
	std::cout << r << std::endl;
	return 0;
}