template<typename T>
std::vector<char> encode(const T* data, size_t count) {
    std::vector<char> result;
    result.reserve(count);
    for (auto elem : data) {
        uint64_t curr_border = 32;
        char len_cnt = 0;
        byte = elem;
        while(elem > curr_border) {
            ++len_cnt;
            curr_border *= 256;
        }
        result.push
    }
    return result;
}

template<typename T>
std::vector<T> decode(const char* data, size_t length);
