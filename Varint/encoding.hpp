#pragma once
#include <vector>
#include <iostream>
#include <bitset>

template<typename T>
std::vector<char> encode(const T* data, size_t count) {
    std::vector<char> result;
    result.reserve(count);
    for (size_t ind = 0; ind < count; ++ind) {
        T elem = data[ind];
        std::vector<char> encoded_elem;
        char len_cnt = 0;
        while (elem >= 32) {
            ++len_cnt;
            encoded_elem.push_back(elem % 256);
            elem >>= 8;
        }
        len_cnt <<= 5;
        encoded_elem.push_back(elem | len_cnt);
        for (size_t code_ind = 0; code_ind < encoded_elem.size(); ++code_ind) {
            result.push_back(encoded_elem[encoded_elem.size() - code_ind - 1]);
        }
    }
    return result;
}

template<typename T>
std::vector<T> decode(const char* data, size_t length) {
    std::vector<T> result;
    size_t ind = 0;
    while (ind < length) {
        uint8_t len = (uint8_t(data[ind]) >> 5) + 1;
        T elem = 31 & data[ind];
        for (char code_ind = 1; code_ind < len; ++code_ind) {
            elem <<= 8;
            elem += uint8_t(data[ind + code_ind]);
        }
        result.push_back(elem);
        ind += len;
    }
    return result;
}
