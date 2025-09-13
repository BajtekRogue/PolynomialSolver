#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iomanip>
#include <iostream>
#include <string>

class Logger {
public:

//  Global disable flag - overrides individual settings
#ifdef ENABLE_ALL_LOGGING
    #define ENABLE_GROEBNER_LOGGING 1
    #define ENABLE_SOLVER_LOGGING 1
    #define ENABLE_CHARACTERISTIC_LOGGING 1
    #define ENABLE_PROGRESS_BAR 1
#endif

//  Default values if not specified by compiler flags
#ifndef ENABLE_GROEBNER_LOGGING
    #define ENABLE_GROEBNER_LOGGING 1
#endif

#ifndef ENABLE_SOLVER_LOGGING
    #define ENABLE_SOLVER_LOGGING 1
#endif

#ifndef ENABLE_CHARACTERISTIC_LOGGING
    #define ENABLE_CHARACTERISTIC_LOGGING 1
#endif

#ifndef ENABLE_PROGRESS_BAR
    #define ENABLE_PROGRESS_BAR 1
#endif

    constexpr static bool enabled_groebnerBasis = ENABLE_GROEBNER_LOGGING;
    constexpr static bool enabled_solver = ENABLE_SOLVER_LOGGING;
    constexpr static bool enabled_characteristicEq = ENABLE_CHARACTERISTIC_LOGGING;
    constexpr static bool enabled_progressBar = ENABLE_PROGRESS_BAR;

    static void groebnerBasis(const std::string& message) {
        if constexpr (enabled_groebnerBasis) {
            std::cout << message << std::endl;
        }
    }

    static void solver(const std::string& message) {
        if constexpr (enabled_solver) {
            std::cout << message << std::endl;
        }
    }

    static void characteristicEq(const std::string& message) {
        if constexpr (enabled_characteristicEq) {
            std::cout << message << std::endl;
        }
    }

    static void printProgressBar(int current, int total, int barWidth = 50) {
        if constexpr (enabled_progressBar) {
            float progress = static_cast<float>(current) / total;
            int pos = static_cast<int>(barWidth * progress);

            std::cout << "\r[";
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) {
                    std::cout << "█";
                }
                else if (i == pos) {
                    std::cout << "▌";
                }
                else {
                    std::cout << " ";
                }
            }
            std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "% ("
                      << current << "/" << total << ")";
            std::cout.flush();
        }
    }

    static void clearProgressBar() {
        if constexpr (enabled_progressBar) {
            std::cout << "\r" << std::string(80, ' ') << "\r";
            std::cout.flush();
        }
    }
};

#endif //  LOGGER_HPP