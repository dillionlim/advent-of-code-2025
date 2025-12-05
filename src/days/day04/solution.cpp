#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <queue>

struct Day04 : public Solution {
    constexpr static int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    constexpr static int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    std::vector<std::string> parse_input(const std::string& input) {
        std::vector<std::string> ans;
        std::istringstream iss(input);
        std::string line;
        while(std::getline(iss, line, '\n')) {
            ans.push_back(line);
        }
        return ans;
    }

    bool in_bounds(int x, int y, int maxx, int maxy) {
        return (x >= 0 && y >= 0 && x < maxx && y < maxy);
    }

    std::string part1(const std::string& input) override {
        std::vector<std::string> in = parse_input(input);
        int ans = 0;
        for (int i = 0; i < in.size(); i++) {
            for (int j = 0; j < in[0].size(); j++) {
                if (in[i][j] == '@') {
                    int cnt = 0;
                    for (int k = 0; k < 8; k++) {
                        int x = i + dx[k];
                        int y = j + dy[k];
                        if (in_bounds(x, y, in.size(), in[0].size()) && in[x][y] == '@') {
                            cnt++;
                        }
                    }
                    ans += (cnt < 4);
                }
            }
        }

        return std::to_string(ans);
    }

    std::string part2(const std::string& input) override {
        std::queue<std::pair<int, int>> q;
        std::vector<std::string> in = parse_input(input);
        std::vector<std::vector<short>> neighbours(in.size(), std::vector<short>(in[0].size(), 0));
        std::vector<std::vector<bool>> vis(in.size(), std::vector<bool>(in[0].size(), false));
        for (int i = 0; i < in.size(); i++) {
            for (int j = 0; j < in[0].size(); j++) {
                if (in[i][j] == '@') {
                    int cnt = 0;
                    for (int k = 0; k < 8; k++) {
                        int x = i + dx[k];
                        int y = j + dy[k];
                        if (in_bounds(x, y, in.size(), in[0].size()) && in[x][y] == '@') {
                            neighbours[i][j]++;
                            cnt++;
                        }
                    }
                    if (cnt < 4) {
                        vis[i][j] = true;
                        q.emplace(i, j);
                    }
                }
            }
        }

        int ans = 0;
        while (!q.empty()) {
            ans += q.size();
            int sz = q.size();
            for (int m = 0; m < sz; m++) {
                int i = q.front().first;
                int j = q.front().second;
                q.pop();
                for (int k = 0; k < 8; k++) {
                    int x = i + dx[k];
                    int y = j + dy[k];
                    if (in_bounds(x, y, in.size(), in[0].size()) && !vis[x][y] && in[x][y] == '@') {
                        neighbours[x][y]--;
                        if (neighbours[x][y] < 4) {
                            q.emplace(x, y);
                            vis[x][y] = true;
                        }
                    }
                }
            }
        }

        return std::to_string(ans);
    }
};

AOC_REGISTER(4, Day04)
