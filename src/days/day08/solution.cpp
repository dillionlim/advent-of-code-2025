#include "aoc.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <cmath>
#include <stdexcept>

struct Day08 : public Solution {
    struct Point {
        int id;
        long long val[3];
    };

    struct Edge {
        long long distSq;
        int u, v;
    };

    struct ParsedInput {
        int num_connections;
        std::vector<Point> points;
    };

    class KDTree {
        struct Node {
            Point point;
            Node *left = nullptr, *right = nullptr;
            int axis;
            Node(Point pt, int ax) : point(pt), axis(ax) {}
        };

        Node* root = nullptr;

        long long distSq(const Point& a, const Point& b) const {
            long long d = 0;
            for (int i = 0; i < 3; i++) {
                long long diff = a.val[i] - b.val[i];
                d += diff * diff;
            }
            return d;
        }

        Node* build(std::vector<Point>& pts, int depth) {
            if (pts.empty()) return nullptr;
            int axis = depth % 3;
            int mid = pts.size() / 2;
            std::nth_element(pts.begin(), pts.begin() + mid, pts.end(),
                [axis](const Point& a, const Point& b) {
                    return a.val[axis] < b.val[axis];
                });

            Node* node = new Node(pts[mid], axis);
            std::vector<Point> left_pts(pts.begin(), pts.begin() + mid);
            std::vector<Point> right_pts(pts.begin() + mid + 1, pts.end());
            
            node->left = build(left_pts, depth + 1);
            node->right = build(right_pts, depth + 1);
            return node;
        }

        void query(Node* node, const Point& target, int k, 
                   std::priority_queue<std::pair<long long, int>>& pq) {
            if (!node) return;
            long long d = distSq(node->point, target);
            
            if (d > 0) {
                pq.push({d, node->point.id});
                if ((int)pq.size() > k) pq.pop();
            }

            long long diff = target.val[node->axis] - node->point.val[node->axis];
            Node* near = (diff < 0) ? node->left : node->right;
            Node* far  = (diff < 0) ? node->right : node->left;

            query(near, target, k, pq);

            long long diffSq = diff * diff;
            if ((int)pq.size() < k || diffSq < pq.top().first) {
                query(far, target, k, pq);
            }
        }

        void destroy(Node* n) {
            if (!n) return;
            destroy(n->left);
            destroy(n->right);
            delete n;
        }

    public:
        KDTree(std::vector<Point> pts) { root = build(pts, 0); }
        ~KDTree() { destroy(root); }

        std::vector<std::pair<long long, int>> findNearest(const Point& target, int k) {
            std::priority_queue<std::pair<long long, int>> pq;
            query(root, target, k, pq);
            std::vector<std::pair<long long, int>> result;
            while(!pq.empty()) {
                result.push_back(pq.top());
                pq.pop();
            }
            return result;
        }
    };

    struct DSU {
        std::vector<int> parent, size;
        DSU(int n) {
            parent.resize(n);
            size.resize(n, 1);
            for(int i=0; i<n; ++i) parent[i] = i;
        }
        int find(int i) {
            if(parent[i] == i) return i;
            return parent[i] = find(parent[i]);
        }
        bool unite(int i, int j) {
            int root_i = find(i);
            int root_j = find(j);
            if(root_i != root_j) {
                if(size[root_i] < size[root_j]) std::swap(root_i, root_j);
                parent[root_j] = root_i;
                size[root_i] += size[root_j];
                return true;
            }
            return false;
        }
        int getSize(int i) { return size[find(i)]; }
    };

    ParsedInput parse_input(const std::string& input) {
        ParsedInput data;
        std::stringstream ss(input);
        std::string line;
        if (std::getline(ss, line)) {
            data.num_connections = std::stoi(line);
        }
        int id_counter = 0;
        while (std::getline(ss, line)) {
            if (line.empty()) continue;
            long long x, y, z;
            char c;
            std::stringstream ls(line);
            if (ls >> x >> c >> y >> c >> z) {
                data.points.push_back({id_counter++, {x, y, z}});
            }
        }
        return data;
    }

    std::string part1(const std::string& input) override {
        ParsedInput data = parse_input(input);
        int N = data.points.size();
        long long max_edges = (long long)N * (N - 1) / 2;
        int limit = data.num_connections;
        if (limit > max_edges) limit = max_edges;
        KDTree tree(data.points);
        std::vector<Edge> candidates;
        candidates.reserve(N * 10);
        int k_neighbors = 10; 
        for (const auto& p : data.points) {
            auto neighbors = tree.findNearest(p, k_neighbors);
            for (auto& n : neighbors) {
                if (p.id < n.second) {
                    candidates.push_back({n.first, p.id, n.second});
                }
            }
        }
        if ((int)candidates.size() > limit) {
            std::partial_sort(candidates.begin(), 
                              candidates.begin() + limit, 
                              candidates.end(), 
                              [](const Edge& a, const Edge& b) {
                                  return a.distSq < b.distSq;
                              });
        } else {
            std::sort(candidates.begin(), candidates.end(), 
                [](const Edge& a, const Edge& b) { return a.distSq < b.distSq; });
        }

        DSU dsu(N);
        int edges_to_process = std::min((int)candidates.size(), limit);
        
        for (int i = 0; i < edges_to_process; ++i) {
            dsu.unite(candidates[i].u, candidates[i].v);
        }

        std::vector<long long> sizes;
        std::vector<bool> visited(N, false);
        
        for (int i = 0; i < N; ++i) {
            int root = dsu.find(i);
            if (!visited[root]) {
                sizes.push_back(dsu.getSize(root));
                visited[root] = true;
            }
        }

        std::sort(sizes.rbegin(), sizes.rend());

        long long result = 1;
        if (sizes.size() >= 3) {
            result = sizes[0] * sizes[1] * sizes[2];
        } else {
            for (auto s : sizes) result *= s;
        }

        return std::to_string(result);
    }

    std::string part2(const std::string& input) override {
        ParsedInput data = parse_input(input);
        int N = data.points.size();
        if (N == 0) return "0";
        KDTree tree(data.points);
        int k_neighbors = 10; 
        std::vector<Edge> candidates;
        candidates.reserve(N * k_neighbors);

        for (const auto& p : data.points) {
            auto neighbors = tree.findNearest(p, k_neighbors);
            for (auto& n : neighbors) {
                if (p.id < n.second) {
                    candidates.push_back({n.first, p.id, n.second});
                }
            }
        }

        std::sort(candidates.begin(), candidates.end(), 
            [](const Edge& a, const Edge& b) { return a.distSq < b.distSq; });

        DSU dsu(N);
        int distinct_components = N;

        for (const auto& edge : candidates) {
            if (dsu.unite(edge.u, edge.v)) {
                distinct_components--;
                if (distinct_components == 1) {
                    long long x1 = data.points[edge.u].val[0];
                    long long x2 = data.points[edge.v].val[0];
                    return std::to_string(x1 * x2);
                }
            }
        }

        std::runtime_error("Error: Graph not fully connected with chosen K");
    }
};

AOC_REGISTER(8, Day08)