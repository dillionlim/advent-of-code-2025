#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>

struct Day11 : public Solution {
    std::unordered_map<std::string, std::vector<std::string>> parse_input(const std::string& input) {
        std::stringstream ss(input);
        std::string line;
        std::unordered_map<std::string, std::vector<std::string>> ans;

        while (std::getline(ss, line)) {
            size_t pos = line.find(':');
            std::string key = line.substr(0, pos);
            std::string rest = line.substr(pos + 1);
            std::stringstream ls(rest);
            std::string val;
            std::vector<std::string> val_list;

            while (ls >> val) {
                val_list.push_back(val);
            }

            ans[key] = val_list;
        }

        return ans;
    }

    std::string part1(const std::string& input) override {
        std::unordered_map<std::string, std::vector<std::string>> in = parse_input(input);

        std::queue<std::string> q;
        q.push("you");
        int ans = 0;
        while (!q.empty()) {
            auto temp = q.front();
            q.pop();
            for(const auto &i : in[temp]) {
                if (i == "out") ans++;
                else q.push(i);
            }
        }

        return std::to_string(ans);
    }

    long long solve(std::unordered_map<std::string, std::vector<std::string>>& in, std::string start, std::string end) {
        std::unordered_map<std::string, int> indeg;
        std::unordered_map<std::string, long long> counts;
        for (const auto& pair : in) {
            indeg[pair.first];
            for (const auto& neighbor : pair.second) {
                indeg[neighbor]++;
            }
        }
        indeg[start] = 0; 
        std::queue<std::string> q;
        for (const auto& pair : indeg) {
            if (pair.second == 0) {
                q.push(pair.first);
            }
        }
        counts[start] = 1;
        while (!q.empty()) {
            std::string u = q.front();
            q.pop();
            if (in.count(u)) {
                for (const auto& v : in[u]) {
                    counts[v] += counts[u];
                    indeg[v]--;
                    if (indeg[v] == 0) {
                        q.push(v);
                    }
                }
            }
        }
        return counts[end];
    }

    std::string part2(const std::string& input) override {
        std::unordered_map<std::string, std::vector<std::string>> in = parse_input(input);

        return std::to_string(solve(in, "svr", "fft") * solve(in, "fft", "dac") * solve(in, "dac", "out") + 
            solve(in, "svr", "dac") * solve(in, "dac", "fft") * solve(in, "fft", "out"));
    }
};

AOC_REGISTER(11, Day11)
