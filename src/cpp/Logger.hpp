#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>

class Logger {
public:
    constexpr static bool enabled = false;

    static void log(const std::string& message) {
        if constexpr (enabled) {
            std::cout << message << std::endl;
        }
    }

    static void error(const std::string& message) {
        if constexpr (enabled) {
            std::cerr << message << std::endl;
        }
    }
};

#endif //  LOGGER_HPP