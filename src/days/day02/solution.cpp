#include "aoc.hpp"
#include <vector>
#include <algorithm>
#include <sstream>

struct Day02 : public Solution {
    using DP = std::vector<std::vector<std::vector<std::pair<long long, long long>>>>;
    
    constexpr static long long POWS[] = {
        1LL, 10LL, 100LL, 1000LL, 10000LL, 
        100000LL, 1000000LL, 10000000LL, 100000000LL, 1000000000LL,
        10000000000LL, 100000000000LL, 1000000000000LL, 10000000000000LL,
        100000000000000LL, 1000000000000000LL, 10000000000000000LL, 100000000000000000LL,
        1000000000000000000LL
    };

    std::vector<std::pair<long long, long long>> parse_input(const std::string& input) {
        using namespace std::literals::string_view_literals; 
        std::vector<std::pair<long long, long long>> ans;
        std::istringstream iss(input);
        std::string line;
        while(std::getline(iss, line, ',')) {
            try {
                if (line.empty()) continue;
                size_t dash = line.find('-');
                if (dash != std::string::npos) {
                    ans.emplace_back(std::stoll(line.substr(0, dash)), std::stoll(line.substr(dash + 1)));
                }
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid number format in line: " << line << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Number out of range in line: " << line << std::endl;
            }
        }
        return ans;
    }

    std::pair<long long, long long> rec(int idx, bool flag, bool leading_zeros, const std::string &limit, DP &dp) {
        if (idx == limit.size()) {
            return {1, 0};
        }
        if (dp[idx][flag][leading_zeros].first != -1) {
            return dp[idx][flag][leading_zeros];
        }

        long long lim = flag ? (limit[idx] - '0') : 9;
        long long count = 0;
        long long sum = 0;

        for (int digit = 0; digit <= lim; digit++) {
            bool is_flag = flag && (digit == lim);
            bool is_leading = leading_zeros && (digit == 0);
            std::pair<long long, long long> result = rec(idx + 1, is_flag, is_leading, limit, dp);
            long long cnt = result.first;
            long long tot = result.second;
            if (!is_leading) {
                count += cnt;
                sum += (digit * POWS[limit.size() - 1 - idx] * cnt) + tot;
            } else {
                count += cnt;
                sum += tot;
            }
        }
        
        return dp[idx][flag][leading_zeros] = {count, sum};
    }

    long long get_sum_up_to(long long n) {
        if (n < 0) return 0;
        if (n == 0) return 0;
        std::string s = std::to_string(n);
        DP dp(s.size() + 1, std::vector<std::vector<std::pair<long long, long long>>>(2, std::vector<std::pair<long long, long long>>(2, {-1, -1})));
        return rec(0, true, true, s, dp).second;
    }

    long long solve(long long limit) {
        std::string s = std::to_string(limit);
        long long ans = 0;
        int maxlen = s.length();
        long long currpow = 10;
        for(int i = 1; i * 2 <= maxlen; i++) {
            long long mult = currpow + 1;
            long long maxi;
            if (i * 2 < maxlen) {
                maxi = currpow - 1;
            }
            else {
                long long prefix = std::stoll(s.substr(0, i));
                long long suffix = std::stoll(s.substr(i));
                if (prefix > suffix) {
                    maxi = prefix - 1;
                } else {
                    maxi = prefix;
                }
            }
            
            long long mini = currpow / 10;
            if (maxi >= mini) {
                long long sum = get_sum_up_to(maxi) - get_sum_up_to(mini - 1);
                ans += sum * mult;
            }
            if (i * 2 < maxlen) {
                currpow *= 10; 
            }
        }
        return ans;
    }

    std::string part1(const std::string& input) override {
        std::vector<std::pair<long long, long long>> in = parse_input(input);
        long long ans = 0;
        for (auto &[lo, hi] : in) {
            ans += solve(hi) - solve(lo - 1);
        }
        return std::to_string(ans);
    }

    long long sum_range(long long start, long long end) {
        if (start > end) return 0;
        long long count = end - start + 1LL;
        long long sum = start + end;
        return (sum * count) / 2LL;
    }

    long long expand(int period_len, int tot_len) {
        long long mult = 0;
        long long curr = 1;
        for (int i = 0; i < tot_len / period_len; ++i) {
            mult += curr;
            curr *= POWS[period_len];
        }
        return mult;
    }

    long long get_valid_sum(long long limit, int len) {
        long long min_val = POWS[len - 1];
        if (limit < min_val) return 0;
        long long total = sum_range(min_val, limit);
        for (int i = 1; i <= len / 2; i++) {
            if (len % i == 0) {
                std::string lim = std::to_string(limit);
                std::string pref = lim.substr(0, i);
                long long start_lim = std::stoll(pref);
                std::string reconstructed = "";
                for(int j = 0; j < len / i; j++) {
                    reconstructed += pref;
                }
                if (reconstructed.length() > lim.length() || 
                    (reconstructed.length() == lim.length() && reconstructed > lim)) {
                    start_lim--;
                }
                long long sum_seeds = get_valid_sum(start_lim, i);
                total -= sum_seeds * expand(i, len);
            }
        }
        return total;
    }

    long long solve_invalid(long long limit) {
        if (limit < 10) return 0;
        std::string s = std::to_string(limit);
        int maxlen = s.length();
        long long ans = 0;

        for (int i = 2; i <= maxlen; i++) {
            long long curr;
            if (i < maxlen) {
                curr = 0;
                for(int j=0; j < i; j++) {
                    curr = curr * 10 + 9;
                }
            } 
            else {
                curr = limit;
            }
            long long min_val = POWS[i - 1];
            long long sum_all = sum_range(min_val, curr);
            long long sum_valid = get_valid_sum(curr, i);
            ans += (sum_all - sum_valid);
        }
        return ans;
    }

    std::string part2(const std::string& input) override {
        std::vector<std::pair<long long, long long>> in = parse_input(input);
        long long ans = 0;
        for (auto &[lo, hi] : in) {
            ans += solve_invalid(hi) - solve_invalid(lo - 1);
        }
        return std::to_string(ans);
    }
};

AOC_REGISTER(2, Day02)
