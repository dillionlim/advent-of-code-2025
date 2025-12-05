#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cassert>

struct Day05 : public Solution {
    struct Range {
        long long start, end;
        bool operator<(const Range& other) const {
            return start < other.start;
        }
        long long size() const {
            return end - start + 1;
        }
    };

    std::pair<std::vector<Range>, std::vector<long long>> parse_input(const std::string& input) {
        std::vector<Range> ranges;
        std::vector<long long> ids;
        
        std::stringstream ss(input);
        std::string line;
        bool parsing_ranges = true;
        while (std::getline(ss, line)) {
            if (line.empty()) {
                parsing_ranges = false;
                continue;
            }
            if (parsing_ranges) {
                long long s, e;
                char dash;
                std::stringstream ls(line);
                if (ls >> s >> dash >> e) {
                    ranges.push_back({s, e});
                }
            } else {
                try {
                    ids.push_back(std::stoll(line));
                } 
                catch (...) { 
                    continue; 
                }
            }
        }
        return {ranges, ids};
    }

    std::vector<Range> merge_intervals(std::vector<Range> &input) {
        std::vector<Range> merged;
        assert(!input.empty());
        merged.push_back(input[0]);
        for (int i = 1; i < input.size(); i++) {
            if (input[i].start <= merged.back().end) {
                merged.back().end = std::max(merged.back().end, input[i].end);
            } 
            else {
                merged.push_back(input[i]);
            }
        }
        return merged;
    }

    std::string part1(const std::string& input) override {
        auto [ranges, queries] = parse_input(input);
        std::sort(ranges.begin(), ranges.end());
        ranges = merge_intervals(ranges);

        int ans = 0;
        for (long long id : queries) {
            Range key = {id, id};
            auto it = std::upper_bound(ranges.begin(), ranges.end(), key);
            if (it != ranges.begin()) {
                auto check = std::prev(it);
                if (id <= check -> end) {
                    ans ++;
                }
            }
        }

        return std::to_string(ans);
    }

    std::string part2(const std::string& input) override {
        auto [ranges, queries] = parse_input(input);
        std::sort(ranges.begin(), ranges.end());
        ranges = merge_intervals(ranges);

        return std::to_string(std::accumulate(ranges.begin(), ranges.end(), 0LL, 
            [](long long acc, Range &x) { return acc + x.size(); }));
    }
};

AOC_REGISTER(5, Day05)
