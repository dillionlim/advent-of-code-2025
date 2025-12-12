#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <array>
#include <map>
#include <set>
#include <bit> 
#include <future>
#include <mutex>
#include <memory>
#include <cstring>

struct Day12 : public Solution {
    using Coord = std::pair<int, int>;
    using MaskType = std::array<uint64_t, 40>;
    using MoveType = std::pair<int, MaskType>;
    using FlatLookup = std::pair<std::vector<MoveType>, std::vector<int>>;
    using RegionType = std::pair<std::pair<int, int>, std::vector<int>>;

    static constexpr int MASK_wORDS = 40;

    static constexpr void mask_set(MaskType& m, int idx) {
        m[idx >> 6] |= (1ULL << (idx & 63));
    }

    static constexpr bool mask_overlaps(const MaskType& a, const MaskType& b) {
        for (int i = 0; i < MASK_wORDS; i++) if (a[i] & b[i]) return true;
        return false;
    }

    static constexpr void mask_combine(MaskType& dest, const MaskType& src) {
        for (int i = 0; i < MASK_wORDS; i++) dest[i] |= src[i];
    }

    static constexpr int mask_first_zero(const MaskType& m, int hint_idx, int max_bit) {
        int word_idx = hint_idx >> 6;
        
        if (word_idx < MASK_wORDS) {
            uint64_t w = ~m[word_idx]; 
            w &= (~0ULL << (hint_idx & 63));
            if (w) return (word_idx << 6) + std::countr_zero(w);
        }

        for (int i = word_idx + 1; i < MASK_wORDS; i++) {
            if ((i << 6) >= max_bit) return max_bit;
            if (~m[i]) return (i << 6) + std::countr_zero(~m[i]);
        }
        return max_bit;
    }

    static std::vector<std::vector<Coord>> generate_orientations(const std::vector<Coord>& cells) {
        std::set<std::vector<Coord>> uniq;
        for (int rot = 0; rot < 4; ++rot) {
            for (int flip = 0; flip < 2; ++flip) {
                std::vector<Coord> coords;
                coords.reserve(cells.size());
                int minx = 9999, miny = 9999;
                for (auto [x, y] : cells) {
                    int rx = x, ry = y;
                    if (rot == 1) { rx = -y; ry = x; }
                    else if (rot == 2) { rx = -x; ry = -y; }
                    else if (rot == 3) { rx = y; ry = -x; }
                    if (flip) rx = -rx;
                    coords.emplace_back(rx, ry);
                    if (rx < minx) minx = rx;
                    if (ry < miny) miny = ry;
                }
                for (auto &p : coords) {
                    p.first -= minx;
                    p.second -= miny;
                }
                std::sort(coords.begin(), coords.end());
                uniq.insert(coords);
            }
        }
        return {uniq.begin(), uniq.end()};
    }

    static void parse_input(const std::string& input, std::vector<std::vector<Coord>>& shapes, std::vector<RegionType>& regions) {
        std::stringstream ss(input);
        std::string line;
        std::map<int, std::vector<std::string>> shapeGrids;
        bool inRegions = false;
        int curr = -1;
        std::vector<std::string> shape;

        while (std::getline(ss, line)) {
            if (line.empty()) continue;
            
            if (!inRegions && line.find(':') != std::string::npos) {
                 std::string prefix = line.substr(0, line.find(':'));
                 if (prefix.find('x') != std::string::npos) inRegions = true;
            }

            if (inRegions) {
                auto pos = line.find(':');
                std::string dims = line.substr(0, pos);
                int xpos = dims.find('x');
                int w = std::stoi(dims.substr(0, xpos));
                int h = std::stoi(dims.substr(xpos + 1));
                std::stringstream ss2(line.substr(pos + 1));
                std::vector<int> counts;
                int v;
                while (ss2 >> v) counts.push_back(v);
                regions.push_back({{w, h}, counts});
            } 
            else {
                auto pos = line.find(':');
                if (pos != std::string::npos) {
                    if (curr != -1) shapeGrids[curr] = shape;
                    curr = std::stoi(line.substr(0, pos));
                    shape.clear();
                } 
                else shape.push_back(line);
            }
        }
        if (curr != -1) shapeGrids[curr] = shape;
        if (shapeGrids.empty()) return;
        shapes.assign(shapeGrids.rbegin()->first + 1, {});
        for (auto& [idx, grid] : shapeGrids) {
            for (int y = 0; y < (int)grid.size(); ++y) {
                for (int x = 0; x < (int)grid[y].size(); ++x) {
                    if (grid[y][x] == '#') shapes[idx].emplace_back(x, y);
                }
            }
        }
    }

    static std::shared_ptr<FlatLookup> build_lookup(int w, int h, const std::vector<std::vector<Coord>>& shapes, const std::vector<std::vector<std::vector<Coord>>>& shapeOrients) {
        int boardArea = w * h;
        std::vector<std::vector<MoveType>> tempMoves(boardArea);

        for (int i= 0; i< shapes.size(); i++) {
            for (const auto& orient : shapeOrients[i]) {
                int maxx = 0, maxy = 0;
                for (auto p : orient) {
                    maxx = std::max(maxx, p.first);
                    maxy = std::max(maxy, p.second);
                }
                for (int j = 0; j <= h - (maxy + 1); j++) {
                    for (int k = 0; k <= w - (maxx + 1); k++) {
                        MaskType m = {0};
                        int first_bit = 9999;
                        for (auto p : orient) {
                            int bit = (j + p.second) * w + (k + p.first);
                            mask_set(m, bit);
                            if (bit < first_bit) first_bit = bit;
                        }
                        if (first_bit < boardArea)
                            tempMoves[first_bit].push_back({i, m});
                    }
                }
            }
        }

        for (int i = 0; i < boardArea; i++) {
            MaskType m = {0};
            mask_set(m, i);
            tempMoves[i].push_back({shapes.size(), m});
        }

        std::vector<int> areas;
        areas.reserve(shapes.size() + 1);
        for(const auto& sh : shapes) areas.push_back((int)sh.size());
        areas.push_back(1); 

        for(auto& moves : tempMoves) {
            std::sort(moves.begin(), moves.end(), [&](const MoveType& a, const MoveType& b) {
                return areas[a.first] > areas[b.first];
            });
        }

        auto lookupPtr = std::make_shared<FlatLookup>();
        auto& allMoves = lookupPtr->first;
        auto& offsets = lookupPtr->second;
        
        offsets.resize(boardArea + 1);
        int currentOffset = 0;
        for (int i = 0; i < boardArea; i++) {
            offsets[i] = currentOffset;
            currentOffset += tempMoves[i].size();
        }
        offsets[boardArea] = currentOffset;
        
        allMoves.reserve(currentOffset);
        for (const auto& vec : tempMoves) {
            allMoves.insert(allMoves.end(), vec.begin(), vec.end());
        }

        return lookupPtr;
    }

    static bool solve_recursive(const FlatLookup& lookup, MaskType current_mask, std::vector<int>& remaining_counts, int start_cell, int board_limit) {
        int cell = mask_first_zero(current_mask, start_cell, board_limit);
        
        if (cell >= board_limit) return true;

        const auto& allMoves = lookup.first;
        const auto& offsets = lookup.second;
        
        const MoveType* it = &allMoves[offsets[cell]];
        const MoveType* end = &allMoves[offsets[cell+1]];
        
        for (; it != end; it++) {
            int shapeIdx = it->first;
            const auto& moveMask = it->second;

            if (remaining_counts[shapeIdx] > 0) {
                if (!mask_overlaps(current_mask, moveMask)) {
                    
                    remaining_counts[shapeIdx]--;
                    MaskType next_mask = current_mask;
                    mask_combine(next_mask, moveMask);

                    if (solve_recursive(lookup, next_mask, remaining_counts, cell + 1, board_limit)) {
                        return true;
                    }

                    remaining_counts[shapeIdx]++;
                }
            }
        }
        return false;
    }

    std::string part1(const std::string& input) override {
        std::vector<std::vector<Coord>> shapes;
        std::vector<RegionType> regions;
        parse_input(input, shapes, regions);
        
        if (shapes.empty()) return "0";

        std::vector<std::vector<std::vector<Coord>>> shapeOrients(shapes.size());
        for (int i = 0; i < shapes.size(); i++) shapeOrients[i] = generate_orientations(shapes[i]);

        std::set<std::pair<int,int>> uniqueSizes;
        for(const auto& reg : regions) uniqueSizes.insert(reg.first);

        std::map<std::pair<int,int>, std::shared_ptr<FlatLookup>> geometryCache;
        std::mutex cacheMutex;
        
        std::vector<std::future<void>> cacheFutures;
        for (const auto& size : uniqueSizes) {
            cacheFutures.push_back(std::async(std::launch::async, [&]() {
                auto lookup = build_lookup(size.first, size.second, shapes, shapeOrients);
                std::lock_guard<std::mutex> lock(cacheMutex);
                geometryCache[size] = lookup;
            }));
        }
        for (auto& f : cacheFutures) f.get();

        std::vector<std::future<bool>> futures;
        futures.reserve(regions.size());

        for (const auto& reg : regions) {
            futures.push_back(std::async(std::launch::async, 
                [&geometryCache, &shapes, reg]() -> bool {
                    int w = reg.first.first;
                    int h = reg.first.second;
                    const auto& countsIn = reg.second;
                    int boardArea = w * h;

                    long long totalShapeArea = 0;
                    for (int i = 0; i < shapes.size(); i++) totalShapeArea += (long long)countsIn[i] * shapes[i].size();
                    
                    if (totalShapeArea > boardArea) return false;
                    if (boardArea > 2500) return false;

                    int slack = boardArea - (int)totalShapeArea;
                    const auto& lookupPtr = geometryCache.at({w, h});

                    MaskType emptyMask = {0};
                    std::vector<int> counts = countsIn;
                    
                    if (slack > 0) {
                        if ((int)counts.size() <= shapes.size()) counts.resize(shapes.size() + 1, 0);
                        counts[shapes.size()] = slack;
                    } 
                    else if ((int)counts.size() <= shapes.size()) counts.resize(shapes.size() + 1, 0);

                    return solve_recursive(*lookupPtr, emptyMask, counts, 0, boardArea);
                }
            ));
        }

        int ans = 0;
        for (auto& f : futures) {
            if (f.get()) ans++;
        }
        return std::to_string(ans);
    }

    std::string part2(const std::string& input) override {
        return "todo";
    }
};

AOC_REGISTER(12, Day12)