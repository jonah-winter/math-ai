#include <iostream>
#include <concepts>
#include "lut_tanh.h"
#include "lut_log.h"
template <typename T>
struct lazy_t {
    T &val;
    lazy_t(const T &val) : val(&val) {}
    template <typename U>
    requires std::constructible_from<U, T>
    operator U() const {
        return static_cast<U>(&val);
    }
    constexpr T operator()() const {
        return val;
    }
};


int main() {
    std::cout << "Hello World\n";

    return 0;
}