#ifndef CONFIG_H
#define CONFIG_H

constexpr bool DEBUG = false;
constexpr bool LOG = true;

constexpr int OR_OPT_LENGTH_LIMIT = 3;
constexpr int OR_OPT_LENGTH_LIMIT_IN_THREE_OPT = -1;
constexpr int CROSS_LENGTH_LIMIT = 3;

constexpr double gls_lambda = 0.1;
constexpr int TIME_LIMIT = 10;

#endif // CONFIG_H