#include "aoc.hpp"
#include <vector>
#include <utility>
#include <sstream>
#include <cassert>

struct Day01 : public Solution {
    std::vector<std::pair<bool, int>> parse_input(const std::string& input) {
        std::vector<std::pair<bool, int>> ans;
        std::istringstream iss(input);
        std::string line;
        while(std::getline(iss, line, '\n')) {
            assert(line.length() == 2 || line.length() == 3);
            assert(line[0] == 'L' || line[0] == 'R');
            try {
                bool direction = line[0] == 'L';
                int distance = std::stoi(line.substr(1));
                ans.emplace_back(direction, distance);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid number format in line: " << line << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Number out of range in line: " << line << std::endl;
            }
        }
        return ans;
    }
    std::string part1(const std::string& input) override {
        std::vector<std::pair<bool, int>> in = parse_input(input); // [L = True / R = False, Number]
        int curr = 50;
        int ans = 0;
        for (const auto &[left, number] : in) {
            curr += ((left) ? -1 : 1) * number;
            curr = (curr + 100) % 100;
            ans += (curr == 0);
        }
        return std::to_string(ans);
    }

    std::string part2(const std::string& input) override {
        return "todo";
    }
};

AOC_REGISTER(1, Day01)
