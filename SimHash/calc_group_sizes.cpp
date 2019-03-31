#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <exception>
#include <bitset>
#include <cstring>
#include <chrono>


size_t MAX_DISTANCE = 3;
size_t PREPROCESSING_REPORT_PERIOD = 50000;
size_t ALGO_REPORT_PERIOD = 20000;

struct PreprocessedData {
    std::unordered_map<uint64_t, size_t> hash_counts;
    std::vector<std::unordered_map<uint16_t, std::unordered_set<uint64_t>>> inverted_indexes;
};
PreprocessedData preprocessing();
std::unordered_map<uint64_t, size_t> calc_group_sizes(const PreprocessedData& data);
void dump_group_sizes(const std::unordered_map<uint64_t, size_t>& sizes);

std::vector<uint16_t> get_parts(uint64_t simhash) {
    std::vector<uint16_t> parts(MAX_DISTANCE + 1);
    std::memcpy(parts.data(), &simhash, sizeof(simhash));
    return parts;
}

bool check_distance(uint64_t alpha, uint64_t beta) {
    return std::bitset<64>(alpha ^ beta).count() <= MAX_DISTANCE;
}

int main() {
    auto data = preprocessing();
    size_t sum = 0;
    for (auto x : data.hash_counts) {
        sum += x.second;
    }
    std::cout << "joke " << sum << std::endl;
    auto sizes = calc_group_sizes(data);
    dump_group_sizes(sizes);
    return 0;
}

PreprocessedData preprocessing() {
    PreprocessedData result;
    result.inverted_indexes.resize(4);
    std::ifstream simhash_file("wiki_simhash.txt");
    std::cout << "begin preprocessing" << std::endl;
    if (simhash_file.is_open()) {
        size_t line_ind = 0;
        auto start = std::chrono::steady_clock::now();
        std::string line;
        while (std::getline(simhash_file, line)) {
            uint64_t simhash = std::stoul(line);
            if (result.hash_counts.find(simhash) == result.hash_counts.end()) {
                result.hash_counts[simhash] = 1;
            } else {
                result.hash_counts[simhash] += 1;
            }
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

std::unordered_map<uint64_t, size_t> calc_group_sizes(const PreprocessedData& data) {
    std::cout << "start algo" << std::endl;
    auto start = std::chrono::steady_clock::now();
    size_t hash_ind = 0;
    std::unordered_map<uint64_t, size_t> sizes;
    for (const auto& simhash_count : data.hash_counts) {
        uint64_t simhash = simhash_count.first;
        if (sizes.find(simhash) != sizes.end()) {
            continue;
        }
        auto parts = get_parts(simhash);
        std::unordered_set<uint64_t> curr_group({simhash});
        for (size_t ind = 0; ind < MAX_DISTANCE + 1; ++ind) {
            for (auto candidate : data.inverted_indexes.at(ind).at(parts[ind])) {
                if (check_distance(simhash, candidate)) {
                    curr_group.insert(candidate);
                }
            }
        }
        sizes[simhash] = 0;
        for (auto group_member : curr_group) {
            sizes[simhash] += data.hash_counts.at(group_member);
        }

        ++hash_ind;
        if (hash_ind % ALGO_REPORT_PERIOD == 0) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start);
            std::cout << hash_ind << " hashes passed, it took ";
            std::cout << duration.count() << " seconds " << std::endl;
        }
    }
    std::cout << "end algo" << std::endl;
    return sizes;
}

void dump_group_sizes(const std::unordered_map<uint64_t, size_t>& sizes) {
    std::cout << "write group size stats to the output file..." << std::endl;
    std::ofstream output_file("group_sizes.txt");
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
