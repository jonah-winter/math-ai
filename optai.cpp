#include <iostream>
#include "lut_log.h"
#include <chrono>
#include <array>
#include <arm_neon.h>
#include <initializer_list>
#include <algorithm>
#include <random>
//use lazy, vectorization(simd), bit shifting, memoization
//use int16 return type but int32 inside functions

namespace {
  namespace mathFuncs {
    int16_t q115(const double x) {
      const long product = std::lround(x * 32768.0);
      // std::clamp didn't work, so I did std::min(std::max(prod, low), high)
      return static_cast<int16_t>(std::min(std::max(product, -32768L), 32767L));
    }
    double unq115(const int16_t x) {
      return x / 32768.0;
    }
    int16_t qMult(const int16_t x, const int16_t y) {
      return static_cast<int16_t>((static_cast<int32_t>(x) * static_cast<int32_t>(y)) >> 15);
    }
    int16_t ln(const int16_t x) {
      const auto inter1 {lnLUT[(x >> 6) & 255]};
      const auto inter2 {lnLUT[((x >> 6) & 255) + 1]};
      const int16_t remain {static_cast<int16_t>(x & 63)};
      return static_cast<int16_t>(inter1 + (((inter2 - inter1) * remain) >> 6));
    }
    int16_t leakyRelu(const int16_t x) {
      return x > 0 ? x : static_cast<int16_t>(x >> 6);
    }
    //int16_t ln(int16_t x) {
    //  int findDistance {__builtin_clz(x)};
    //
    //}
  };
  template <size_t ro, size_t co, typename T = int16_t>
  struct Matrix {
      /*
       *  This is to make each matrix be able to access each others members, as they are
       *  treated as different types.
       */
      template <size_t r, size_t c, typename U>
      friend struct Matrix;
      static constexpr size_t rows = ro;
      static constexpr size_t cols = co;
      static constexpr size_t x8rows = (rows + 7) & ~7;
      static constexpr size_t x8cols = (cols + 15) & ~15;
      static constexpr int rowremain = x8rows - rows;
      static constexpr int colremain = x8cols - cols;
    private:
      struct uninitialized {};
      explicit Matrix(uninitialized) {};
      alignas(64) std::array<T, x8rows * x8cols> listOfNums;
    public:
      static size_t size() {
        return rows * cols;
      }
      [[nodiscard]] const std::array<T, x8rows * x8cols> &getlist() const {
        return this->listOfNums;
      }
      const T& operator[](const size_t r, const size_t c) const {
        return this->listOfNums[r * x8cols + c];
      }
      T& operator[](const size_t r, const size_t c) {
        return this->listOfNums[r * x8cols + c];
      }
      T& operator[](const size_t i) {
        const size_t r = i / cols;
        const size_t c = i % cols;
        return this->listOfNums[r * x8cols + c];
      }
      const T& operator[](const size_t i) const {
        const size_t r = i / cols;
        const size_t c = i % cols;
        return this->listOfNums[r * x8cols + c];
      }
      friend std::ostream& operator<<(std::ostream& os, const Matrix &m) {
        os << '[';
        for (size_t r{0}; r < rows; ++r) {
          os << '[';
          for (size_t c{0}; c < cols; ++c) {
            os << m[r, c];
            if (c + 1 < cols) {
              os << ", ";
            }
          }
          os << ']';
          if (r + 1 < rows) {
            os << ", ";
          }
        }
        os << ']';
        return os;
      }
      Matrix(const Matrix &m) = default;
      Matrix() {
        listOfNums.fill(0);
      }
      // this function uses SIMD to transpose quickly
      using transposed = Matrix<co, ro, T>;
      [[nodiscard]] transposed transpose() const {
        transposed newlof{typename transposed::uninitialized{}};
        if constexpr (rows < 8 || cols < 16) {
          for (size_t r{0}; r < rows; ++r) {
            for (size_t c{0}; c < cols; ++c) {
              newlof[c, r] = (*this)[r, c];
            }
          }
          return newlof;
        }
        else {
          #pragma unroll
          for (int r{0}; r < x8rows; r+=8) {
            #pragma unroll
            for (int c{0}; c < x8cols; c+=16) {
              __builtin_prefetch(&this->listOfNums[(r + 8) * x8cols + c], 0, 3);
              int16x8_t v[8], x[8];
              #pragma unroll
              for (int i{0}; i < 8; ++i) {
                v[i] = vld1q_s16(&this->listOfNums[(r + i) * x8cols + c]);
                // works every time because cols is padded with at least 16 elements
                x[i] = vld1q_s16(&this->listOfNums[(r + i) * x8cols + (c + 8)]);
              }
              // 16 bit passes; transposes with 2x2 blocks
              const int16x8_t load01 = vtrn1q_s16(v[0], v[1]); const int16x8_t loadx01 = vtrn1q_s16(x[0], x[1]);
              const int16x8_t load11 = vtrn2q_s16(v[0], v[1]); const int16x8_t loadx11 = vtrn2q_s16(x[0], x[1]);
              v[0] = load01; v[1] = load11;
              x[0] = loadx01; x[1] = loadx11;
              const int16x8_t load31 = vtrn2q_s16(v[2], v[3]); const int16x8_t loadx31 = vtrn2q_s16(x[2], x[3]);
              const int16x8_t load21 = vtrn1q_s16(v[2], v[3]); const int16x8_t loadx21 = vtrn1q_s16(x[2], x[3]);
              v[2] = load21; v[3] = load31; x[2] = loadx21; x[3] = loadx31;
              const int16x8_t load41 = vtrn1q_s16(v[4], v[5]); const int16x8_t loadx41 = vtrn1q_s16(x[4], x[5]);
              const int16x8_t load51 = vtrn2q_s16(v[4], v[5]); const int16x8_t loadx51 = vtrn2q_s16(x[4], x[5]);
              v[4] = load41; v[5] = load51; x[4] = loadx41; x[5] = loadx51;
              const int16x8_t load61 = vtrn1q_s16(v[6], v[7]); const int16x8_t loadx61 = vtrn1q_s16(x[6], x[7]);
              const int16x8_t load71 = vtrn2q_s16(v[6], v[7]); const int16x8_t loadx71 = vtrn2q_s16(x[6], x[7]);
              v[6] = load61; v[7] = load71; x[6] = loadx61; x[7] = loadx71;
              // 32 bit passes; transposes those 2x2 blocks with 4x4 blocks
              // i hate this
              const int32x4_t load0_2 = vtrn1q_s32(vreinterpretq_s32_s16(v[0]), vreinterpretq_s32_s16(v[2]));
              const int32x4_t load1_2 = vtrn2q_s32(vreinterpretq_s32_s16(v[0]), vreinterpretq_s32_s16(v[2]));
              const int32x4_t loadx0_2 = vtrn1q_s32(vreinterpretq_s32_s16(x[0]), vreinterpretq_s32_s16(x[2]));
              const int32x4_t loadx1_2 = vtrn2q_s32(vreinterpretq_s32_s16(x[0]), vreinterpretq_s32_s16(x[2]));
              v[0] = vreinterpretq_s16_s32(load0_2); v[2] = vreinterpretq_s16_s32(load1_2);
              x[0] = vreinterpretq_s16_s32(loadx0_2); x[2] = vreinterpretq_s16_s32(loadx1_2);
              const int32x4_t load2_2 = vtrn1q_s32(vreinterpretq_s32_s16(v[1]), vreinterpretq_s32_s16(v[3]));
              const int32x4_t load3_2 = vtrn2q_s32(vreinterpretq_s32_s16(v[1]), vreinterpretq_s32_s16(v[3]));
              const int32x4_t loadx2_2 = vtrn1q_s32(vreinterpretq_s32_s16(x[1]), vreinterpretq_s32_s16(x[3]));
              const int32x4_t loadx3_2 = vtrn2q_s32(vreinterpretq_s32_s16(x[1]), vreinterpretq_s32_s16(x[3]));
              v[1] = vreinterpretq_s16_s32(load2_2); v[3] = vreinterpretq_s16_s32(load3_2);
              x[1] = vreinterpretq_s16_s32(loadx2_2); x[3] = vreinterpretq_s16_s32(loadx3_2);
              const int32x4_t load4_2 = vtrn1q_s32(vreinterpretq_s32_s16(v[4]), vreinterpretq_s32_s16(v[6]));
              const int32x4_t load5_2 = vtrn2q_s32(vreinterpretq_s32_s16(v[4]), vreinterpretq_s32_s16(v[6]));
              const int32x4_t loadx4_2 = vtrn1q_s32(vreinterpretq_s32_s16(x[4]), vreinterpretq_s32_s16(x[6]));
              const int32x4_t loadx5_2 = vtrn2q_s32(vreinterpretq_s32_s16(x[4]), vreinterpretq_s32_s16(x[6]));
              v[4] = vreinterpretq_s16_s32(load4_2); v[6] = vreinterpretq_s16_s32(load5_2);
              x[4] = vreinterpretq_s16_s32(loadx4_2); x[6] = vreinterpretq_s16_s32(loadx5_2);
              const int32x4_t load6_2 = vtrn1q_s32(vreinterpretq_s32_s16(v[5]), vreinterpretq_s32_s16(v[7]));
              const int32x4_t load7_2 = vtrn2q_s32(vreinterpretq_s32_s16(v[5]), vreinterpretq_s32_s16(v[7]));
              const int32x4_t loadx6_2 = vtrn1q_s32(vreinterpretq_s32_s16(x[5]), vreinterpretq_s32_s16(x[7]));
              const int32x4_t loadx7_2 = vtrn2q_s32(vreinterpretq_s32_s16(x[5]), vreinterpretq_s32_s16(x[7]));
              v[5] = vreinterpretq_s16_s32(load6_2); v[7] = vreinterpretq_s16_s32(load7_2);
              x[5] = vreinterpretq_s16_s32(loadx6_2); x[7] = vreinterpretq_s16_s32(loadx7_2);
              // 64 bit passes; transposes those 4x4 blocks with 8x8 blocks
              const int64x2_t load0_3 = vtrn1q_s64(vreinterpretq_s64_s16(v[0]), vreinterpretq_s64_s16(v[4]));
              const int64x2_t load1_3 = vtrn2q_s64(vreinterpretq_s64_s16(v[0]), vreinterpretq_s64_s16(v[4]));
              const int64x2_t loadx0_3 = vtrn1q_s64(vreinterpretq_s64_s16(x[0]), vreinterpretq_s64_s16(x[4]));
              const int64x2_t loadx1_3 = vtrn2q_s64(vreinterpretq_s64_s16(x[0]), vreinterpretq_s64_s16(x[4]));
              v[0] = vreinterpretq_s16_s64(load0_3); v[4] = vreinterpretq_s16_s64(load1_3);
              x[0] = vreinterpretq_s16_s64(loadx0_3); x[4] = vreinterpretq_s16_s64(loadx1_3);
              const int64x2_t load2_3 = vtrn1q_s64(vreinterpretq_s64_s16(v[1]), vreinterpretq_s64_s16(v[5]));
              const int64x2_t load3_3 = vtrn2q_s64(vreinterpretq_s64_s16(v[1]), vreinterpretq_s64_s16(v[5]));
              const int64x2_t loadx2_3 = vtrn1q_s64(vreinterpretq_s64_s16(x[1]), vreinterpretq_s64_s16(x[5]));
              const int64x2_t loadx3_3 = vtrn2q_s64(vreinterpretq_s64_s16(x[1]), vreinterpretq_s64_s16(x[5]));
              v[1] = vreinterpretq_s16_s64(load2_3); v[5] = vreinterpretq_s16_s64(load3_3);
              x[1] = vreinterpretq_s16_s64(loadx2_3); x[5] = vreinterpretq_s16_s64(loadx3_3);
              const int64x2_t load4_3 = vtrn1q_s64(vreinterpretq_s64_s16(v[2]), vreinterpretq_s64_s16(v[6]));
              const int64x2_t load5_3 = vtrn2q_s64(vreinterpretq_s64_s16(v[2]), vreinterpretq_s64_s16(v[6]));
              const int64x2_t loadx4_3 = vtrn1q_s64(vreinterpretq_s64_s16(x[2]), vreinterpretq_s64_s16(x[6]));
              const int64x2_t loadx5_3 = vtrn2q_s64(vreinterpretq_s64_s16(x[2]), vreinterpretq_s64_s16(x[6]));
              v[2] = vreinterpretq_s16_s64(load4_3); v[6] = vreinterpretq_s16_s64(load5_3);
              x[2] = vreinterpretq_s16_s64(loadx4_3); x[6] = vreinterpretq_s16_s64(loadx5_3);
              const int64x2_t load6_3 = vtrn1q_s64(vreinterpretq_s64_s16(v[3]), vreinterpretq_s64_s16(v[7]));
              const int64x2_t load7_3 = vtrn2q_s64(vreinterpretq_s64_s16(v[3]), vreinterpretq_s64_s16(v[7]));
              const int64x2_t loadx6_3 = vtrn1q_s64(vreinterpretq_s64_s16(x[3]), vreinterpretq_s64_s16(x[7]));
              const int64x2_t loadx7_3 = vtrn2q_s64(vreinterpretq_s64_s16(x[3]), vreinterpretq_s64_s16(x[7]));
              v[3] = vreinterpretq_s16_s64(load6_3); v[7] = vreinterpretq_s16_s64(load7_3);
              x[3] = vreinterpretq_s16_s64(loadx6_3); x[7] = vreinterpretq_s16_s64(loadx7_3);
              #pragma unroll
              for (int i{0}; i < 8; ++i) {
                vst1q_s16(&newlof.listOfNums[(c + i) * transposed::x8cols + r], v[i]);
                vst1q_s16(&newlof.listOfNums[(c + i + 8) * transposed::x8cols + r], x[i]);
              }
            }
          }
        return newlof;
      }
      }
      template <size_t newcols>
      [[nodiscard]] Matrix<rows, newcols, T> operator*(const Matrix<cols, newcols, T> &x) const {
        auto transX {x.transpose()};
        Matrix<rows, newcols, T> finalfull;
        #pragma unroll
        for (auto r{0}; r < x8rows; r+=4) {
          #pragma unroll
          for (auto c{0}; c < decltype(transX)::x8rows; c+=4) {
            int32x4_t finallow[4][4];
            int16x4_t ylow[4];
            int16x4_t zlow[4];
            __builtin_prefetch(&this->listOfNums[(r + 8) * x8cols + c], 0, 3);
            #pragma unroll
            for (int k{0}; k < 4; ++k) {
              #pragma unroll
              for (int u{0}; u < 4; ++u) {
                finallow[k][u]  = vdupq_n_s32(0);
              }
            }
            for (int p{0}; p < x8cols; p+=4) {
              #pragma unroll
              for (int i{0}; i < 4; ++i) {
                if (r + i < rows) {
                  ylow[i] = vld1_s16(&this->listOfNums[(r + i) * x8cols + p]);
                } else {
                  ylow[i] = vdup_n_s16(0);
                }
                if (c + i < transX.rows) {
                  zlow[i] = vld1_s16(&transX.listOfNums[(c + i) * decltype(transX)::x8cols + p]);
                } else {
                  zlow[i] = vdup_n_s16(0);
                }
              }
              #pragma unroll
              for (int k{0}; k < 4; ++k) {
                #pragma unroll
                for (int u{0}; u < 4; ++u) {
                  finallow[k][u]  = vmlal_s16(finallow[k][u], ylow[k], zlow[u]);
                }
              }
            }
            #pragma unroll
            for (int row{0}; row < 4; ++row) {
              #pragma unroll
              for (int col{0}; col < 4; ++col) {
                if ((r + row < rows) && (c + col < newcols)) {
                  int32_t sum =
                    vaddvq_s32(finallow[row][col]);
                  finalfull[r + row, c + col] = static_cast<int16_t>(
                    std::min(std::max(sum >> 15, -32768), 32767)
                    );
                }
              }
            }
          }
        }
        return finalfull;
      }
      Matrix(std::initializer_list<std::initializer_list<T>> list)
      : Matrix() {
        size_t r {0};
        for (const auto &row : list) {
          size_t c {0};
          for (const auto &val : row) {
            if (r < rows && c < cols) {
              (*this)[r, c] = val;
            }
            ++c;
          }
          ++r;
        }
      }
  };
  template <typename func, typename any, typename any2>
  void getTime(func oper, any parA, any2 parB) {
    auto teststart {std::chrono::steady_clock::now()};
    volatile auto testval = oper(parA, parB);
    auto testend {std::chrono::steady_clock::now()};
    auto testfin = std::chrono::duration_cast<std::chrono::nanoseconds>(testend - teststart);
    int x {0};
    if (testfin.count() < 4000.0) {
      x = 100000.0;
    }
    else {
      x = 10000.0;
    }
    auto start {std::chrono::steady_clock::now()};
    for (int i{0}; i < x; ++i) {
      asm volatile("" ::: "memory");
      auto val = oper(parA, parB);
      asm volatile("" : "+m"(val) :: "memory");
    }
    auto end {std::chrono::steady_clock::now()};
    auto fin {std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) / static_cast<double>(x)};
    std::cout << fin.count() << "ns\n";
  }
  template <typename func, typename any>
  void getTime(func oper, any parA) {
    auto teststart {std::chrono::steady_clock::now()};
    volatile auto testval = oper(parA);
    auto testend {std::chrono::steady_clock::now()};
    auto testfin {std::chrono::duration_cast<std::chrono::nanoseconds>(testend - teststart) / 100000.0};
    int x {0};
    if (testfin.count() < 4000.0) {
      x = 100000.0;
    }
    else {
      x = 10000.0;
    }
    auto start {std::chrono::steady_clock::now()};
    for (int i{0}; i < x; ++i) {
      asm volatile("" ::: "memory");
      auto val = oper(parA);
      asm volatile("" : "+m"(val) :: "memory");
    }
    auto end {std::chrono::steady_clock::now()};
    auto fin {std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) / static_cast<double>(x)};
    std::cout << fin.count() << "ns\n";
  }
  template <typename func, typename empty>
  void getTime(func oper) {
    auto teststart {std::chrono::steady_clock::now()};
    empty a {oper()};
    auto testend {std::chrono::steady_clock::now()};
    auto testfin {std::chrono::duration_cast<std::chrono::nanoseconds>(testend - teststart)};
    int x{0};
    if (testfin.count() < 4000.0) {
      x = 100000.0;
    }
    else {
      x = 10000.0;
    }
    auto start {std::chrono::steady_clock::now()};
    for (int i{0}; i < x; ++i) {
      asm volatile("" ::: "memory");
      auto val = oper();
      asm volatile("" : "+m"(val) :: "memory");
    }
    auto end {std::chrono::steady_clock::now()};
    auto fin {std::chrono::duration_cast<std::chrono::nanoseconds>(end - start) / static_cast<double>(x)};
    std::cout << fin.count() << "ns\n";
  }
}
int main() {
  using namespace mathFuncs; using namespace std::chrono;
  Matrix<256, 256> a, b;
  for (int r{0}; r < 256; ++r) {
    for (int c{0}; c < 256; ++c) {
      a[r, c] = q115((r - 128.0) / 200.0 + (c / 2000.0));
      b[r, c] = q115((r - 128.0) / 200.0 - (c / 2000.0));
    }
  }
  //getTime([](const Matrix<256, 256> &b) {return b.transpose();}, a);
  //std::cout << a.transpose() << '\n';
  //std::cout << (a * b)[1] << '\n';
  getTime(std::multiplies<>{}, a, b);
  return 0;
}