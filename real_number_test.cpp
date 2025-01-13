#include <real_number.hpp>

int main() {
	real_number::real_number r{2};
	r = sqrt(r);
	r = r + sqrt(real_number::real_number{2});
	r = r * sqrt(real_number::real_number{ 2 });
    std::cout << r << std::endl;
    std::cout << "deduce(r) = " << deduce_0(r) << std::endl;
    std::cout << "deduce_1(deduce_0(r)) =" << deduce_1(deduce_0(r)) << std::endl;
	return 0;
}
