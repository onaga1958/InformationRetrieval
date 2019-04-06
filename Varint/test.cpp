#include <random>
#include <iostream>
#include <bitset>
#include "encoding.hpp"


int main() {
    std::mt19937 gen(0);
    std::vector<std::uniform_int_distribution<uint64_t>> distributions = {
        std::uniform_int_distribution<uint64_t>(0, 30),
        std::uniform_int_distribution<uint64_t>(40, 8191),
        std::uniform_int_distribution<uint64_t>(8192, 2097151),
        std::uniform_int_distribution<uint64_t>(4294967296, 281474976710656),
    };
    size_t N = 10000;
    for (auto distr : distributions) {
        std::vector<uint64_t> numbers;
        numbers.reserve(N);
        for (size_t ind = 0; ind < N; ++ind) {
            numbers.push_back(distr(gen));
        }
        auto encoded = encode<uint64_t>(numbers.data(), numbers.size());
        auto decoded = decode<uint64_t>(encoded.data(), encoded.size());
        if (decoded.size() != numbers.size()) {
            std::cerr << "Different sizes: " << numbers.size() << " and " << decoded.size() << std::endl;
        } else {
            for (size_t ind = 0; ind < numbers.size(); ++ind) {
                if (numbers[ind] != decoded[ind]) {
                    std::cerr << "Error: " << numbers[ind] << ", " << decoded[ind] << std::endl;
                }
            }
        }
    }
    return 0;
}
