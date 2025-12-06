#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <cassert>
#include <numeric>

struct Day06 : public Solution {
    std::pair<std::vector<char>, std::vector<std::vector<long long>>> parse_input(const std::string& input) {
        std::vector<char> ops;
        std::vector<std::vector<long long>> vals;
        
        std::stringstream ss(input);
        std::string line;
        while (std::getline(ss, line)) {
            std::string in;
            std::stringstream liness(line);
            std::vector<long long> nums;
            try {
                while (liness >> in) {
                    if (!std::isdigit(in[0])) {
                        ops.push_back(in[0]);
                    }
                    else {
                        nums.push_back(std::stoll(in));
                    }
                } 
            } catch (const std::out_of_range& oor) {
                std::cerr << "Out of range";
            } catch (const std::invalid_argument& ia) {
                std::cerr << "Invalid arguments";
            }
            
            if (!nums.empty()) {
                vals.push_back(nums);
            }
        }
        return {ops, vals};
    } 

    std::string part1(const std::string& input) override {
        auto [ops, vals] = parse_input(input);
        long long ans = 0;
        for (int i = 0; i < ops.size(); i++) {
            long long sum = (ops[i] == '+') ? 0 : 1;
            for (int j = 0; j < vals.size(); j++) {
                if (ops[i] == '+') sum += vals[j][i];
                else sum *= vals[j][i];
            }
            ans += sum;
        }
        return std::to_string(ans);
    }

    std::pair<std::vector<char>, std::vector<std::vector<long long>>> parse_input_vert(const std::string& input) {
        std::vector<char> ops;
        std::vector<std::vector<long long>> vals;
        std::vector<std::string> grid;

        std::stringstream ss(input);
        std::string line;
        int maxi=0;
        while (std::getline(ss, line)) {
            grid.push_back(line);
            maxi = std::max(maxi, (int) line.length());
        }
        for (auto &row : grid) row.resize(maxi, ' ');

        std::vector<long long> curr_block;
        char curr_op = ' ';
        bool has_num = false; 
        for (int i = 0; i < maxi; i++) {
            std::string curr_num = "";
            bool is_separator = true;
            for (int j = 0; j < grid.size(); j++) {
                char c = grid[j][i];
                if (c != ' ') {
                    is_separator = false;
                    if (isdigit(c)) {
                        curr_num += c;
                    } 
                    else {
                        assert(c == '*' || c == '+');
                        curr_op = c;
                    }
                }
            }
            if (!curr_num.empty()) {
                curr_block.push_back(std::stoll(curr_num));
                has_num = true;
            }

            if ((is_separator || i == maxi - 1) && has_num) {
                if (is_separator && !curr_block.empty()) {
                    ops.push_back(curr_op);
                    vals.push_back(curr_block);
                    curr_block.clear();
                    curr_op = ' '; 
                    has_num = false;
                }
                else if (i == maxi - 1 && !curr_block.empty()) {
                     ops.push_back(curr_op);
                     vals.push_back(curr_block);
                }
            }
        }
        
        return {ops, vals};
    }

    std::string part2(const std::string& input) override {
        auto [ops, vals] = parse_input_vert(input);
        long long ans = 0;
        for (int i = 0; i < ops.size(); i++) {
            if (ops[i] == '+') ans += std::accumulate(vals[i].begin(), vals[i].end(), 0LL);
            else ans += std::accumulate(vals[i].begin(), vals[i].end(), 1LL, 
                [](long long acc, long long val) { return acc * val; });
        }
        return std::to_string(ans);
    }
};

AOC_REGISTER(6, Day06)
