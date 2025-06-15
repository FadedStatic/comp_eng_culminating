#pragma once
#include <iostream>
#include <vector>
#include <string>

// rshash function from https://github.com/richardwtf/rshash
// i am not qualified to make a good hashing algorithm by myself.
inline unsigned int rshash(const std::string& str)
{
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;

    for(std::size_t i = 0; i < str.length(); i++)
    {
        hash = hash * a + str[i];
        a = a * b;
    }

    return (hash & 0x7FFFFFFF);
}

// levenshtein for string comparison
// actually a really cool method, good for search alg that isn just stringcmp
// src: https://github.com/guilhermeagostinelli/levenshtein/blob/master/levenshtein.cpp
inline int levenshtein(const std::string& s1, const std::string& s2) {
    size_t m = s1.size(), n = s2.size();
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1));

    for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i-1][j] + 1,
                dp[i][j-1] + 1,
                dp[i-1][j-1] + cost
            });
        }
    }

    return dp[m][n];
}

inline double weighted_string_score(const std::string& s1, const std::string& s2) {
    int dist = levenshtein(s1, s2);
    int max_len = std::max(s1.length(), s2.length());
    if (max_len == 0) return 1.0;
    return 1.0 - static_cast<double>(dist) / max_len;
}

inline std::string collect_input_str(const char* fmt) {
    std::printf("%s\r\n", fmt);
    std::string choice;
    while (std::getline(std::cin, choice)) {
        if (choice.empty()) {
            std::printf("Invalid input, please try again.\r\n%s", fmt);
            continue;
        }
        break;
    }
    return choice;
}

template <typename _Ty>
inline _Ty collect_input(int upper, const std::string_view fmt ="Enter choice (0 to exit/return): ") {
    _Ty num = 0;
    std::printf(fmt.data());
    std::string choice;
    while (std::getline(std::cin, choice)) {
        // from_chars is stoi if it wasnt mid (it's noexcept which means no try/catch required)
        // also would like to point out that the [unused, err] is structured binding, so normally it would be var=from_chars(...) with two members, but you can turn the members directly into vars
        const auto [unused, err] = std::from_chars(choice.data(), choice.data() + choice.size(), num);
        if (choice.empty() || err!=std::errc() || num < 0 || num > upper) {
            std::printf("Invalid input, please try again.\r\n%s",fmt.data());
            continue;
        }
        break;
    }
    return num;
}