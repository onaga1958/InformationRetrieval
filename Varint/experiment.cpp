#include <random>
#include <iostream>
#include <bitset>
#include "encoding.hpp"


int main() {
    std::mt19937 gen(0);

    for (float p = 0.5; p > 0.00000000000000001; p /= 2) {
        std::geometric_distribution<uint64_t> distribution(p);
        size_t N = 100000;
        std::vector<uint64_t> numbers;
        numbers.reserve(N);
        for (size_t ind = 0; ind < N; ++ind) {
            numbers.push_back(distribution(gen));
        }
        auto encoded = encode<uint64_t>(numbers.data(), numbers.size());
        std::cout << p << " " << float(numbers.size() * 8) / encoded.size() << std::endl;
    }
    return 0;
}
