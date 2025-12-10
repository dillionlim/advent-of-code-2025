#include "aoc.hpp"
#include <vector>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <future> 

#include "ortools/linear_solver/linear_solver.h"

struct Day10 : public Solution {
    std::vector<std::pair<uint64_t, std::pair<std::vector<uint64_t>, std::vector<int>>>> parse_input(const std::string& input) {
        std::vector<std::pair<uint64_t, std::pair<std::vector<uint64_t>, std::vector<int>>>> ans;
        std::stringstream ss(input);
        std::string line;
        while (std::getline(ss, line)) {
            uint64_t target = 0;
            size_t open_bracket = line.find('[');
            size_t close_bracket = line.find(']');
            if (open_bracket != std::string::npos && close_bracket != std::string::npos) {
                std::string lights = line.substr(open_bracket + 1, close_bracket - open_bracket - 1);
                for (size_t i = 0; i < lights.size(); ++i) {
                    if (lights[i] == '#') {
                        target |= (1ULL << i);
                    }
                }
            }

            size_t current_pos = close_bracket + 1;
            std::vector<uint64_t> buttons;
            while (true) {
                size_t open_paren = line.find('(', current_pos);
                size_t open_brace = line.find('{');
                if (open_paren == std::string::npos || (open_brace != std::string::npos && open_paren > open_brace)) {
                    break;
                }
                
                size_t close_paren = line.find(')', open_paren);
                std::string content = line.substr(open_paren + 1, close_paren - open_paren - 1);
                
                for (auto& c : content) if (c == ',') c = ' ';

                std::stringstream btn_ss(content);
                int light_idx;
                uint64_t button_mask = 0;
                while (btn_ss >> light_idx) {
                    button_mask |= (1ULL << light_idx);
                }
                buttons.push_back(button_mask);

                current_pos = close_paren + 1;
            }

            size_t open_brace = line.find('{');
            size_t close_brace = line.find('}');

            std::vector<int> joltage;
            if (open_brace != std::string::npos && close_brace != std::string::npos) {
                std::string content = line.substr(open_brace + 1, close_brace - open_brace - 1);
                
                for (auto& c : content) if (c == ',') c = ' ';
                
                std::stringstream j_ss(content);
                int val;
                while (j_ss >> val) {
                    joltage.push_back(val);
                }
            }

            ans.push_back({target, {buttons, joltage}});
        }
        return ans;
    }

    std::string part1(const std::string& input) override {
        std::vector<std::pair<uint64_t, std::pair<std::vector<uint64_t>, std::vector<int>>>> in = parse_input(input);

        long long tot = 0;

        for (const auto &i : in) {
            const auto& [target, inner] = i;
            const auto& [buttons, joltage] = inner;
            if (target == 0) continue;

            std::queue<std::pair<uint64_t, int>> q;
            std::unordered_set<uint64_t> vis;
            q.emplace(0, 0);
            vis.insert(0);

            while (!q.empty()) {
                auto [curr, presses] = q.front();
                q.pop();

                if (curr == target) {
                    tot += presses;
                    break;
                }

                for (const auto& btn_mask : buttons) {
                    uint64_t next = curr ^ btn_mask;
                    if (!vis.contains(next)) {
                        vis.insert(next);
                        q.emplace(next, presses + 1);
                    }
                }
            }
        }

        return std::to_string(tot);
    }

    static long long solve(const std::vector<uint64_t>& buttons, const std::vector<int>& joltage) {
        std::unique_ptr<operations_research::MPSolver> solver(
            operations_research::MPSolver::CreateSolver("SCIP"));
        
        if (!solver) return 0;

        solver->SuppressOutput();

        std::vector<const operations_research::MPVariable*> x;
        x.reserve(buttons.size());
        
        for (int i = 0; i < buttons.size(); i++) {
            x.push_back(solver->MakeIntVar(0.0, operations_research::MPSolver::infinity(), ""));
        }

        for (int i = 0; i < joltage.size(); i++) {
            operations_research::MPConstraint* c = 
                solver->MakeRowConstraint(joltage[i], joltage[i]);
            
            for (int j = 0; j < buttons.size(); j++) {
                if ((buttons[j] >> i) & 1ULL) {
                    c->SetCoefficient(x[j], 1);
                }
            }
        }

        operations_research::MPObjective* objective = solver->MutableObjective();
        for (const auto* var : x) objective->SetCoefficient(var, 1);
        objective->SetMinimization();

        const operations_research::MPSolver::ResultStatus result_status = solver->Solve();

        if (result_status == operations_research::MPSolver::OPTIMAL) {
            return (long long)std::round(objective->Value());
        }
        return 0;
    }

    std::string part2(const std::string& input) override {
        auto in = parse_input(input);
        
        std::vector<std::future<long long>> futures;
        futures.reserve(in.size());

        for (const auto &i : in) {
            const auto& inner = i.second;
            const auto& buttons = inner.first;
            const auto& joltage = inner.second;
            futures.push_back(std::async(std::launch::async, solve, buttons, joltage));
        }

        long long tot = 0;
        for (auto& f : futures) {
            tot += f.get();
        }

        return std::to_string(tot);
    }
};

AOC_REGISTER(10, Day10)
