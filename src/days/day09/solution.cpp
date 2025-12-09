#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <algorithm>
#include <set>

struct Day09 : public Solution {
    std::vector<std::pair<int, int>> parse_input(const std::string& input) {
        std::stringstream ss(input);
        std::string line;
        std::vector<std::pair<int, int>> ans;
        while (std::getline(ss, line)) {
            std::stringstream ls(line);
            std::string x, y;
            if (std::getline(ls, x, ',')) {
                if (std::getline(ls, y)) { 
                    ans.emplace_back(std::stoi(x), std::stoi(y));
                }
            }
        }
        return ans;
    }

    std::string part1(const std::string& input) override {
        // n log n possible using https://codeforces.com/blog/entry/128350
        // but constant factors might cause it to be slower due to small input size

        std::vector<std::pair<int, int>> in = parse_input(input);
        long long maxi = 0;
        for (int i = 0; i < in.size(); i++) {
            for (int j = i + 1; j < in.size(); j++) {
                maxi = std::max(maxi, (long long)(std::abs(in[i].first - in[j].first) + 1) * (std::abs(in[i].second - in[j].second) + 1));
            }
        }
        return std::to_string(maxi);
    }

    std::string part2(const std::string& input) override {
        std::vector<std::pair<int, int>> in = parse_input(input);
        std::set<long long> x, y;
        for(auto& p : in) {
            x.insert(p.first);
            y.insert(p.second);
        }
        
        std::vector<long long> xs(x.begin(), x.end());
        std::vector<long long> ys(y.begin(), y.end());

        auto get_x_idx = [&](long long val) { 
            return std::lower_bound(xs.begin(), xs.end(), val) - xs.begin(); 
        };
        auto get_y_idx = [&](long long val) { 
            return std::lower_bound(ys.begin(), ys.end(), val) - ys.begin(); 
        };
        std::vector<std::vector<int>> grid(ys.size() - 1, std::vector<int>(xs.size() - 1, 0));
        for (int i = 0; i < ys.size() - 1; i++) {
            double curry = (ys[i] + ys[i + 1]) / 2.0;
            std::vector<long long> cross;
            for (int j = 0; j < in.size(); j++) {
                std::pair<int, int> a = in[j];
                std::pair<int, int> b = in[(j + 1) % in.size()];
                if (a.first == b.first) {
                    auto [miny, maxy] = std::minmax(a.second, b.second);
                    if (miny < curry && maxy > curry) {
                        cross.push_back(a.first);
                    }
                }
            }
            std::sort(cross.begin(), cross.end());
            for (int j = 0; j < cross.size(); j += 2) {
                long long startx = cross[j];
                long long endx = cross[j+1];
                int starti = get_x_idx(startx);
                int endi = get_x_idx(endx);
                for (int k = starti; k < endi; k++) {
                    grid[i][k] = 1;
                }
            }
        }
        std::vector<std::vector<int>> pref(ys.size(), std::vector<int>(xs.size(), 0));
        for (int i = 0; i < ys.size() - 1; i++) {
            for (int j = 0; j < xs.size() - 1; j++) {
                pref[i + 1][j + 1] = pref[i][j + 1] + pref[i + 1][j] - pref[i][j] + grid[i][j];
            }
        }
        auto count = [&](int c1, int r1, int c2, int r2) {
            return pref[r2][c2] - pref[r1][c2] - pref[r2][c1] + pref[r1][c1];
        };
        long long maxi = 0;

        for (int i = 0; i < in.size(); i++) {
            for (int j = i + 1; j < in.size(); j++) {
                long long area = (long long)(std::abs(in[i].first - in[j].first) + 1) * (std::abs(in[i].second - in[j].second) + 1);
                if (area <= maxi) continue;

                int c1 = get_x_idx(std::min(in[i].first, in[j].first));
                int c2 = get_x_idx(std::max(in[i].first, in[j].first));
                int r1 = get_y_idx(std::min(in[i].second, in[j].second));
                int r2 = get_y_idx(std::max(in[i].second, in[j].second));

                if ((c2 - c1) * (r2 - r1) == count(c1, r1, c2, r2)) maxi = std::max(maxi, area);
            }
        }

        return std::to_string(maxi);
    }
};

AOC_REGISTER(9, Day09)
