#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <chrono>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp> 

#include "aoc.hpp"

const std::string GREEN = "\033[32m";
const std::string RED   = "\033[31m";
const std::string RESET = "\033[0m";
const std::string GRAY  = "\033[90m";
const std::string BOLD  = "\033[1m";

struct Result { std::string val; double ms; bool pass; bool run; };

Result run_part(Solution* sol, int part, std::string in, std::string exp) {
    if (in.empty() && exp.empty()) return {"", 0.0, false, false}; // Skip if no test

    auto start = std::chrono::high_resolution_clock::now();
    std::string out = (part == 1) ? sol->part1(in) : sol->part2(in);
    auto end = std::chrono::high_resolution_clock::now();
    
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    out.erase(out.find_last_not_of(" \n\r\t") + 1);
    
    bool pass = exp.empty() ? true : (out == exp);
    return {out, ms, pass, true};
}

void print_header() {
    std::cout << "╔═════╦══════════════════╦══════════╦════════╦══════════════════╦══════════╦════════╗\n";
    std::cout << "║ Day ║      Part 1      ║   Time   ║ Result ║      Part 2      ║   Time   ║ Result ║\n";
    std::cout << "╠═════╬══════════════════╬══════════╬════════╬══════════════════╬══════════╬════════╣\n";
}

void print_footer() {
    std::cout << "╚═════╩══════════════════╩══════════╩════════╩══════════════════╩══════════╩════════╝\n";
}

void print_row(int day, Result r1, Result r2) {
    auto print_cell = [](std::string s, int w, std::string c = "") {
        if (s.length() > w) s = s.substr(0, w-3) + "...";
        std::cout << " " << c << std::left << std::setw(w) << s << RESET << " ║";
    };

    auto print_check = [](Result r) {
        if (!r.run) std::cout << "        ║";
        else if (r.pass) std::cout << "   " << GREEN << "✔" << RESET << "    ║";
        else        std::cout << "   " << RED   << "✘" << RESET << "    ║";
    };

    auto format_time = [](Result r) {
        if (!r.run) return std::string("");
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << r.ms << "ms";
        return ss.str();
    };

    std::cout << "║";
    std::cout << " " << std::setw(2) << std::setfill('0') << day << std::setfill(' ') << "  ║"; // Day

    print_cell(r1.val, 16);
    print_cell(format_time(r1), 8, GRAY);
    print_check(r1);

    print_cell(r2.val, 16);
    print_cell(format_time(r2), 8, GRAY);
    print_check(r2);
    
    std::cout << "\n";
}

int main() {
    print_header();

    const auto& all_days = SolutionRegistry::instance().getAll();

    for (const auto& [day_num, solution_ptr] : all_days) {
        std::stringstream ss;
        ss << "src/days/day" << std::setw(2) << std::setfill('0') << day_num << "/test.toml";
        
        std::string p1_in, p1_exp, p2_in, p2_exp;
        bool has_p1 = false, has_p2 = false;

        toml::parse_result config = toml::parse_file(ss.str());
        
        if (config) {
            auto cases = config["test_cases"].as_array();
            if (cases) {
                for (auto&& node : *cases) {
                    toml::table* tbl = node.as_table();
                    if (!tbl) continue;

                    int part = tbl->at("part").value_or(1);
                    if (part == 1 && !has_p1) {
                        p1_in = tbl->at("input").value_or("");
                        p1_exp = tbl->at("expected").value_or("");
                        has_p1 = true;
                    } else if (part == 2 && !has_p2) {
                        p2_in = tbl->at("input").value_or("");
                        p2_exp = tbl->at("expected").value_or("");
                        has_p2 = true;
                    }
                }
            }
        }

        Result r1 = run_part(solution_ptr.get(), 1, p1_in, p1_exp);
        Result r2 = run_part(solution_ptr.get(), 2, p2_in, p2_exp);
        
        print_row(day_num, r1, r2);
    }

    print_footer();
    return 0;
}