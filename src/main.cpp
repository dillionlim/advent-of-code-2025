#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <fstream>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp> 

#include "aoc.hpp"

const std::string GREEN = "\033[32m";
const std::string RED   = "\033[31m";
const std::string RESET = "\033[0m";
const std::string GRAY  = "\033[90m";
const std::string YELLOW = "\033[33m";

struct PartResult {
    std::string real_val;
    double real_ms;
    bool has_real;
    
    bool tests_exist;
    bool tests_passed;
    int tests_run_count;
};

std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

std::pair<std::string, double> run_timed(Solution* sol, int part, const std::string& input) {
    if (input.empty()) return {"", 0.0};
    
    auto start = std::chrono::high_resolution_clock::now();
    std::string out = (part == 1) ? sol->part1(input) : sol->part2(input);
    auto end = std::chrono::high_resolution_clock::now();
    
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    out.erase(out.find_last_not_of(" \n\r\t") + 1);
    
    return {out, ms};
}

PartResult process_part(int day_num, Solution* sol, int part_num, const std::string& real_input, toml::array* test_cases) {
    PartResult res;
    res.has_real = !real_input.empty();
    res.real_val = "";
    res.real_ms = 0.0;
    res.tests_exist = false;
    res.tests_passed = true; 
    res.tests_run_count = 0;

    if (res.has_real) {
        auto [val, ms] = run_timed(sol, part_num, real_input);
        res.real_val = val;
        res.real_ms = ms;
    } else {
        res.real_val = "(No Input)";
    }

    if (test_cases) {
        for (auto&& node : *test_cases) {
            toml::table* tbl = node.as_table();
            if (!tbl) continue;

            int case_part = tbl->at("part").value_or(1);
            if (case_part != part_num) continue;

            res.tests_exist = true;
            res.tests_run_count++;

            std::string input = tbl->at("input").value_or("");
            std::string expected = tbl->at("expected").value_or("");

            auto [actual, _] = run_timed(sol, part_num, input);
            if (actual != expected) {
                res.tests_passed = false;
                std::cerr << "Day " << day_num << " Part " << part_num 
                          << " Test Failed. Exp: " << expected 
                          << ", Got: " << actual << "\n";
            }
        }
    }

    return res;
}

void print_header() {
    std::cout << "╔═════╦══════════════════╦══════════╦════════╦══════════════════╦══════════╦════════╗\n";
    std::cout << "║ Day ║      Part 1      ║   Time   ║ Result ║      Part 2      ║   Time   ║ Result ║\n";
    std::cout << "╠═════╬══════════════════╬══════════╬════════╬══════════════════╬══════════╬════════╣\n";
}

void print_footer() {
    std::cout << "╚═════╩══════════════════╩══════════╩════════╩══════════════════╩══════════╩════════╝\n";
}

void print_row(int day, PartResult r1, PartResult r2) {
    auto print_cell = [](std::string s, int w, std::string color = "") {
        if (s.length() > w) s = s.substr(0, w-3) + "...";
        std::cout << " " << color << std::left << std::setw(w) << s << RESET << " ║";
    };

    auto print_check = [](PartResult r) {
        if (!r.tests_exist) {
             std::cout << "   " << GRAY << "-" << RESET << "    ║"; // No tests
        } else if (r.tests_passed) {
             std::cout << "   " << GREEN << "✔" << RESET << "    ║";
        } else {
             std::cout << "   " << RED << "✘" << RESET << "    ║";
        }
    };

    auto format_time = [](PartResult r) {
        if (!r.has_real) return std::string("");
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << r.real_ms << "ms";
        return ss.str();
    };

    std::cout << "║";
    std::cout << " " << std::right << std::setw(2) << std::setfill('0') << day << std::setfill(' ') << "  ║";

    std::string c1 = r1.has_real ? "" : GRAY;
    print_cell(r1.real_val, 16, c1);
    print_cell(format_time(r1), 8, GRAY);
    print_check(r1);

    std::string c2 = r2.has_real ? "" : GRAY;
    print_cell(r2.real_val, 16, c2);
    print_cell(format_time(r2), 8, GRAY);
    print_check(r2);
    
    std::cout << "\n";
}

int main() {
    print_header();

    const auto& all_days = SolutionRegistry::instance().getAll();

    for (const auto& [day_num, solution_ptr] : all_days) {
        std::stringstream ss_dir;
        ss_dir << "src/days/day" << std::setw(2) << std::setfill('0') << day_num;
        std::string dir = ss_dir.str();

        std::string real_a = read_file(dir + "/a.in");
        std::string real_b = read_file(dir + "/b.in");

        toml::array* test_cases = nullptr;
        toml::parse_result config = toml::parse_file(dir + "/test.toml");
        if (config) {
            test_cases = config["test_cases"].as_array();
        }

        PartResult r1 = process_part(day_num, solution_ptr.get(), 1, real_a, test_cases);
        PartResult r2 = process_part(day_num, solution_ptr.get(), 2, real_b, test_cases);
        
        print_row(day_num, r1, r2);
    }

    print_footer();
    return 0;
}