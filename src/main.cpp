#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <sstream>
#include <string>
#include <numeric>
#include <unordered_set>

using Permutation = std::vector<int>;
using PermutationList = std::vector<Permutation>;
using Domains = std::vector<std::vector<uint64_t>>; // 高々Nは15程度だろう

std::vector<std::vector<PermutationList>> all_permutations;

inline uint64_t bit_of(int v) {
    return 1ULL << (v - 1);
}

void generate_permutations(int n) {
    all_permutations.assign(n + 1, std::vector<PermutationList>(n + 1));
    Permutation p(n);
    std::iota(p.begin(), p.end(), 1);

    do {
        int left_seen = 0, right_seen = 0;
        int max_h = 0;
        for (int i = 0; i < n; ++i) {
            if (p[i] > max_h) {
                max_h = p[i];
                left_seen++;
            }
        }
        max_h = 0;
        for (int i = n - 1; i >= 0; --i) {
            if (p[i] > max_h) {
                max_h = p[i];
                right_seen++;
            }
        }
        if (left_seen <= n && right_seen <= n) {
            all_permutations[left_seen][right_seen].push_back(p);
        }
    } while (std::next_permutation(p.begin(), p.end()));
}

Domains calculate_domains(int n, const std::vector<PermutationList>& line_perm, const std::vector<PermutationList>& col_perm) {
    Domains domains(n, std::vector<uint64_t>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            uint64_t line_mask = 0, col_mask = 0;
            for (const auto& p : line_perm[i]) {
                if (!p.empty()) {
                    line_mask |= bit_of(p[j]);
                }
            }
            for (const auto& p : col_perm[j]) {
                if (!p.empty()) {
                    col_mask |= bit_of(p[i]);
                }
            }
            domains[i][j] = line_mask & col_mask;
        }
    }
    return domains;
}

bool is_solved(int n, const std::vector<PermutationList>& line_perm) {
    for(int i = 0; i < n; ++i) {
        if(line_perm[i].size() != 1) return false;
    }
    return true;
}

bool prune_target_perms(
    int n,
    const std::vector<PermutationList>& source_perms,
    std::vector<PermutationList>& target_perms,
    bool is_source_line,
    bool& changed) {
    Domains possible_values(n, std::vector<uint64_t>(n));
    for (int i = 0; i < n; ++i) {
        for (const auto& p_source : source_perms[i]) {
            for (int j = 0; j < n; ++j) {
                if (is_source_line) {
                    possible_values[i][j] |= bit_of(p_source[j]);
                } else {
                    possible_values[j][i] |= bit_of(p_source[j]);
                }
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        size_t original_size = target_perms[i].size();
        target_perms[i].erase(std::remove_if(target_perms[i].begin(), target_perms[i].end(),
            [&](const Permutation& p_target) {
                for (int j = 0; j < n; ++j) {
                    bool is_valid = false;
                    if (is_source_line) {
                        is_valid = (possible_values[j][i] & bit_of(p_target[j])) != 0;
                    } else {
                        is_valid = (possible_values[i][j] & bit_of(p_target[j])) != 0;
                    }
                    if (!is_valid) return true; // to be removed
                }
                return false; // to be kept
            }), target_perms[i].end());

        if (target_perms[i].size() < original_size) {
            changed = true;
        }
        
        if (target_perms[i].empty()) {
            return false;
        }
    }
    return true;
}

bool pruning(int n, std::vector<PermutationList>& line_perm, std::vector<PermutationList>& col_perm) {
    while (true) {
        bool changed = false;
        if (!prune_target_perms(n, line_perm, col_perm, true, changed))
            return false;
        if (!prune_target_perms(n, col_perm, line_perm, false, changed))
            return false;
        if (!changed)
            break;
    }
    return true;
}

bool solve(int n, std::vector<PermutationList>& line_perm, std::vector<PermutationList>& col_perm) {
    if (!pruning(n, line_perm, col_perm)) {
        return false;
    }

    Domains domains = calculate_domains(n, line_perm, col_perm);
    int r = -1, c = -1;
    size_t min_domain_size = n + 1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (__builtin_popcountll(domains[i][j]) > 1
                && static_cast<size_t>(__builtin_popcountll(domains[i][j])) < min_domain_size) {
                min_domain_size = __builtin_popcountll(domains[i][j]);
                r = i;
                c = j;
            }
        }
    }

    if (r == -1) {
        return is_solved(n, line_perm);
    }

    uint64_t candidates = domains[r][c];
    while (candidates) {
        int val = __builtin_ctzll(candidates) + 1;  // 最下位bitの位置 + 1
        candidates &= candidates - 1;
        auto line_perm_backup = line_perm;
        auto col_perm_backup = col_perm;

        line_perm[r].erase(std::remove_if(line_perm[r].begin(), line_perm[r].end(), 
            [=](const Permutation& p){ return p[c] != val; }), line_perm[r].end());
        col_perm[c].erase(std::remove_if(col_perm[c].begin(), col_perm[c].end(), 
            [=](const Permutation& p){ return p[r] != val; }), col_perm[c].end());

        if (solve(n, line_perm, col_perm)) {
            return true;
        }

        line_perm = line_perm_backup;
        col_perm = col_perm_backup;
    }

    return false;
}


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./solver \"c1 c2 ... cn\" (top bottom left right)" << std::endl;
        return 1;
    }

    std::vector<int> clues;
    std::istringstream iss(argv[1]);
    int num;
    while(iss >> num) {
        clues.push_back(num);
    }
    if (iss.fail() && !iss.eof()) {
        std::cerr << "Error: Invalid input format." << std::endl;
        return 1;
    }

    if (clues.size() % 4 != 0 || clues.size() == 0) {
        std::cerr << "Error: Number of clues must be a multiple of 4." << std::endl;
        return 1;
    }
    int n = clues.size() / 4;

    if (n > 64) {
        std::cerr << "Error: N must be <= 64" << std::endl;
        return 1;
    }

    std::cout << "Solving for N=" << n << std::endl;
    generate_permutations(n);

    std::vector<PermutationList> line_perm(n);
    std::vector<PermutationList> col_perm(n);
    for (int i = 0; i < n; ++i) {
        int top    = clues[i];
        int bottom = clues[n + i];
        int left   = clues[2 * n + i];
        int right  = clues[3 * n + i];
        
        if (top < 1 || top > n || bottom < 1 || bottom > n || left < 1 || left > n || right < 1 || right > n) {
            std::cerr << "Error: Clues must be between 1 and " << n << std::endl;
            return 1;
        }
        col_perm[i] = all_permutations[top][bottom];
        line_perm[i] = all_permutations[left][right];
    }
    
    if (solve(n, line_perm, col_perm)) {
        std::cout << "\nSolution Found:" << std::endl;
        for (int i = 0; i < n; ++i) {
            if (line_perm[i].empty() || line_perm[i][0].empty()) {
                std::cout << "Error: Solution contains empty permutation." << std::endl;
                break;
            }
            for (int j = 0; j < n; ++j) {
                std::cout << line_perm[i][0][j] << (j == n - 1 ? "" : " ");
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "No solution found." << std::endl;
    }

    return 0;
}
