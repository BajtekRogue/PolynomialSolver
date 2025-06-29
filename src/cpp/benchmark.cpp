#include "BigRational.hpp"
#include "GaloisField.hpp"
#include "GroebnerBasis.hpp"
#include "Monomial.hpp"
#include "MonomialOrders.hpp"
#include "MultivariatePolynomial.hpp"
#include "Rational.hpp"
#include "Solver.hpp"
#include "UnivariatePolynomial.hpp"

#include <chrono>

int main() {
    auto X = defineVariable<BigRational>('X');
    auto Y = defineVariable<BigRational>('Y');

    std::vector<std::pair<int, int>> V;
    V.push_back({2, 7});
    V.push_back({3, 5});
    V.push_back({2, 9});
    V.push_back({4, 5});
    V.push_back({2, 11});
    V.push_back({3, 7});
    V.push_back({2, 13});
    V.push_back({3, 8});
    //  [0.1 ; 0.6 ; 0.4 ; 0.5 ; 5.7 ; 31.2 ; 90.1 ; 404.0]
    for (auto [n, m] : V) {

        int P = 1 + (1 << n);
        int Q = 1 + (1 << m);
        auto f1 = (X ^ n) + (Y ^ n) - P;
        auto f2 = (X ^ m) + (Y ^ m) - Q;

        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "========== Calculating for n = " << n << " ; m = " << m << "============\n";

        auto _ = solveSystem<BigRational>({f1, f2}, findBigRationalRoots);
        auto solution = std::get<std::vector<std::map<char, BigRational>>>(_);

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Groebner basis calculation for n = " << n << " ; m = " << m << " took "
                  << elapsed.count() << " seconds.\n";

        for (const auto& sol : solution) {
            for (auto [var, val] : sol) {
                std::cout << var << " = " << val << "\n";
            }
            std::cout << std::endl;
        }
    }
}