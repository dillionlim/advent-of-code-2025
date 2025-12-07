#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <cassert>

struct Day07 : public Solution {
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
        std::unordered_set<int> s;
        for (int i = 0; i < in[0].size(); i++) {
            if (in[0][i] == 'S') {
                s.insert(i);
                break;
            }
        }
        int row = 1;
        int ans = 0;
        while (row < in.size()) {
            std::unordered_set<int> next;
            for (const auto curr : s) {
                if (in[row][curr] == '.') {
                    next.insert(curr);
                } 
                else {
                    assert(curr - 1 >= 0 && curr + 1 < in[0].size());
                    ans++;
                    next.insert(curr - 1);
                    next.insert(curr + 1);
                }
            }
            s = next;
            row++;
        }
        return std::to_string(ans);
    }

    std::string part2(const std::string& input) override {
        std::vector<std::string> in = parse_input(input);
        std::unordered_map<int, long long> m;
        for (int i = 0; i < in[0].size(); i++) {
            if (in[0][i] == 'S') {
                m[i] = 1LL;
                break;
            }
        }
        int row = 1;
        while (row < in.size()) {
            std::unordered_map<int, long long> next;
            for (const auto &[curr, tot] : m) {
                if (in[row][curr] == '.') {
                    next[curr] += tot;
                } 
                else {
                    assert(curr - 1 >= 0 && curr + 1 < in[0].size());
                    next[curr - 1] += tot;
                    next[curr + 1] += tot;
                }
            }
            m = next;
            row++;
        }
        long long ans = std::accumulate(m.begin(), m.end(), 0LL, [](long long acc, const auto &p) { return acc + p.second; });
        return std::to_string(ans);
    }
};

AOC_REGISTER(7, Day07)
