#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <cassert>

struct Day03 : public Solution {
    std::vector<std::string> parse_input(const std::string& input) {
        std::vector<std::string> ans;
        std::istringstream iss(input);
        std::string line;
        while(std::getline(iss, line, '\n')) {
            ans.push_back(line);
        }
        return ans;
    }
    std::string part1(const std::string& input) override {
        std::vector<std::string> in = parse_input(input);
        long long tot = 0;
        for (const auto &line : in) {
            int prev_max = 0;
            int maxi = 0;
            for(auto ch : line) {
                int dig = ch - '0';
                maxi = std::max(maxi, prev_max * 10 + dig);
                prev_max = std::max(prev_max, dig);
            }
            tot += (long long) maxi;
        }
        return std::to_string(tot);
    }

    std::string part2(const std::string& input) override {
        std::vector<std::string> in = parse_input(input);
        long long tot = 0;
        for (const auto &line : in) {
            assert(line.length() >= 12);
            int rem_del = line.length() - 12;
            std::string stack;
            for(auto ch : line) {
                while (!stack.empty() && rem_del > 0 && ch > stack.back()) {
                    stack.pop_back();
                    rem_del--;
                }
                stack.push_back(ch);
            }
            std::string ans = stack.substr(0, 12);
            tot += (long long) std::stoll(ans);
        }
        return std::to_string(tot);
    }
};

AOC_REGISTER(3, Day03)
