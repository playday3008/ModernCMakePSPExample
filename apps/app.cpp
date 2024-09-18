#include <modern/lib.hpp>

#include <iostream>
#include <vector>
#include <tuple>
#include <print>

#include <pspkernel.h>

PSP_MODULE_INFO("ModernCMakePSPExample", PSP_MODULE_USER, 1, 0);

int main() {
    std::vector<double> input = {1.2, 2.3, 3.4, 4.5};

    auto [mean, moment] = accumulate_vector(input);

    std::print("Mean: {}, Moment: {}\n", mean, moment);

    return 0;
}
