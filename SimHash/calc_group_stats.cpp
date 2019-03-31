#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <exception>
#include <bitset>
#include <cstring>
#include <chrono>


size_t MAX_DISTANCE = 3;
size_t PREPROCESSING_REPORT_PERIOD = 1000000;
size_t ALGO_REPORT_PERIOD = 10000;

struct PreprocessedData {
    std::set<uint64_t> remaining_hashes;
    std::vector<std::map<uint16_t, std::set<uint64_t>>> inverted_indexes;
};
PreprocessedData preprocessing();
std::map<size_t, size_t> calc_group_sizes(PreprocessedData* data);
void dump_group_sizes(const std::map<size_t, size_t>& sizes);

std::vector<uint16_t> get_parts(uint64_t simhash) {
    std::vector<uint16_t> parts(MAX_DISTANCE + 1);
    std::memcpy(parts.data(), &simhash, 8);
    return parts;
}

bool check_distance(uint64_t alpha, uint64_t beta) {
    return __builtin_popcount(alpha ^ beta) <= MAX_DISTANCE;
}

int main() {
    auto data = preprocessing();
    auto sizes = calc_group_sizes(&data);
    dump_group_sizes(sizes);
    return 0;
}

PreprocessedData preprocessing() {
    PreprocessedData result;
    result.inverted_indexes.resize(4);
    std::ifstream simhash_file("simhash_sorted.txt");
    std::cout << "begin preprocessing" << std::endl;
    if (simhash_file.is_open()) {
        size_t line_ind = 0;
        auto start = std::chrono::steady_clock::now();
        std::string line;
        while (std::getline(simhash_file, line)) {
            uint64_t simhash = std::stoul(line);
            result.remaining_hashes.insert(simhash);
            auto parts = get_parts(simhash);
            for (size_t ind = 0; ind < MAX_DISTANCE + 1; ++ind) {
                result.inverted_indexes[ind][parts[ind]].insert(simhash);
            }
            ++line_ind;
            if (line_ind % PREPROCESSING_REPORT_PERIOD == 0) {
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - start);
                std::cout << line_ind << " lines processed, it took ";
                std::cout << duration.count() << " seconds" << std::endl;
            }
        }
        std::cout << "end preprocessing" << std::endl;
        simhash_file.close();
    } else {
        throw std::runtime_error("can't open simhash file");
    }
    return result;
}

std::map<size_t, size_t> calc_group_sizes(PreprocessedData* data) {
    std::cout << "start algo" << std::endl;
    auto start = std::chrono::steady_clock::now();
    size_t group_cnt = 0;
    std::map<size_t, size_t> sizes;
    while (!data->remaining_hashes.empty()) {
        uint64_t simhash = *(data->remaining_hashes.begin());
        auto parts = get_parts(simhash);
        std::set<uint64_t> curr_group({simhash});
        for (size_t ind = 0; ind < MAX_DISTANCE + 1; ++ind) {
            for (auto candidate : data->inverted_indexes[ind][parts[ind]]) {
                if (check_distance(simhash, candidate)) {
                    curr_group.insert(candidate);
                }
            }
        }

        if (sizes.find(curr_group.size()) == sizes.end()) {
            sizes[curr_group.size()] = 1;
        } else {
            sizes[curr_group.size()] += 1;
        }
        for (auto group_member : curr_group) {
            data->remaining_hashes.erase(group_member);
            auto member_parts = get_parts(group_member);
            for (size_t ind = 0; ind < MAX_DISTANCE + 1; ++ind) {
                data->inverted_indexes[ind][member_parts[ind]].erase(group_member);
            }
        }

        ++group_cnt;
        if (group_cnt % ALGO_REPORT_PERIOD == 0) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start);
            std::cout << "found " << group_cnt << " groups, it took ";
            std::cout << duration.count() << " seconds. There are ";
            std::cout << data->remaining_hashes.size() << " simhashes left" << std::endl;
        }
    }
    std::cout << "end algo" << std::endl;
    return sizes;
}

void dump_group_sizes(const std::map<size_t, size_t>& sizes) {
    std::cout << "write group size stats to the output file..." << std::endl;
    std::ofstream output_file("group_size_distribution.txt");
    if (output_file.is_open()) {
        for (const auto& item : sizes) {
            output_file << item.first << " " << item.second << "\n";
        }
        output_file.close();
    } else {
        throw std::runtime_error("can't open output file");
    }
    std::cout << "dumping done" << std::endl;
}
