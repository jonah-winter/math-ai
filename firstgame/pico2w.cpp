#include "utilities.h"
#include "lut_log.h"
#include "lut_tanh.h"
#include <arm_acle.h>

namespace {
    template <typename T>
    struct lazy_t {
        T val;
        explicit lazy_t(const T &val)
            : val(val) {};
        T eval() const { return val; }
    };
    namespace mathFuncs {
        constexpr int16_t q115(const double x) {
            const long product = std::lround(x * 32768.0);
            // std::clamp didn't work, so I did std::min(std::max(prod, low), high)
            return static_cast<int16_t>(std::min(std::max(product, -32768L), 32767L));
        }
        constexpr double unq115(const int16_t x) {
            return x / 32768.0;
        }
        constexpr int16_t qMult(const int16_t x, const int16_t y) {
            return static_cast<int16_t>((static_cast<int32_t>(x) * static_cast<int32_t>(y)) >> 15);
        }
        constexpr int16_t ln(const int16_t x) {
            const auto inter1 {lnLUT[(x >> 6) & 255]};
            const auto inter2 {lnLUT[((x >> 6) & 255) + 1]};
            const int16_t remain {static_cast<int16_t>(x & 63)};
            return static_cast<int16_t>(inter1 + (((inter2 - inter1) * remain) >> 6));
        }
        constexpr int16_t tanh(const int16_t x) {
            //get bit that says if its positive or negative
            const auto bit     = static_cast<int16_t>(x >> 15);
            const auto absx    = static_cast<uint16_t>((x ^ bit) - bit);
            const size_t index = absx >> 6;
            const auto remain  = static_cast<int16_t>(absx & 63);
            const int32_t fir  = lut_tanh[index];
            const int32_t sec  = lut_tanh[std::min(index + 1, static_cast<size_t>(512))];
            //interpolation
            const auto final = static_cast<int16_t>(fir + (((sec - fir) * remain) >> 6));
            // get original sign from bit
            return (static_cast<int16_t>((final ^ bit) - bit));
        }
    }
    template <size_t r, size_t c, typename T = int16_t>
    struct Matrix {
        template <size_t ro, size_t co, typename U>
        friend struct Matrix;
        static constexpr size_t rows {r};
        static constexpr size_t cols {c};
        static constexpr size_t paddedRows {(r + 7) & ~7};
        static constexpr size_t paddedCols {(c + 15) & ~15};
    private:
        struct uninitialized {};
        explicit Matrix(uninitialized){};
        alignas(64) std::array<T, paddedRows * paddedCols> listOfNums;
    public:
        constexpr Matrix() {listOfNums.fill(0);}
        constexpr Matrix(const Matrix &m) = default;
        constexpr Matrix(std::initializer_list<std::initializer_list<T>> list)
            : Matrix() {
            size_t i{0};
            for (const auto &row : list) {
                size_t z{0};
                for (const auto &val : row) {
                    if (i < rows && z < cols) {
                        (*this)[i, z] = val;
                    }
                    ++z;
                }
                ++i;
            }

        }
        constexpr T& operator[](const size_t i, const size_t z) {
            return this->listOfNums[i * paddedCols + z];
        }
        const T& operator[](const size_t i, const size_t z) const {
            return this->listOfNums[i * paddedCols + z];
        }
        constexpr T& operator[](const size_t i) {
            return this->listOfNums[(i / cols) * paddedCols + (i % cols)];
        }
        constexpr T& operator[](const size_t i) const {
            return this->listOfNums[(i / cols) * paddedCols + (i % cols)];
        }
        [[nodiscard]] constexpr std::array<T, paddedCols * paddedRows> &get() const {
            return this->listOfNums;
        }
        static constexpr size_t size() {
            return rows * cols;
        }
        friend std::ostream& operator<<(std::ostream& os, const Matrix &m) {
            os << '[';
            for (size_t i{0}; i < rows; ++i) {
                os << '[';
                for (size_t z{0}; z < cols; ++z) {
                    os << m[i, z];
                    if (z + 1 < cols) {
                        os << ", ";
                    }
                }
                os << ']';
                if (i + 1 < rows) {
                    os << ", ";
                }
            }
            os << ']';
            return os;
        }
        [[nodiscard]] Matrix<c, r, T> transpose() const {

        }
    };
}

int main() {
    Matrix<2, 2> a{{1, 2}, {3, 4}};
    std::cout << a;
    decltype(a) b {a};
    lazy_t c{2};
    int d {c.eval()};
    std::cout << d << std::endl;
}