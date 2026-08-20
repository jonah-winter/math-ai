#include <iostream>
#include <cmath>
#include <iomanip>
#include <concepts>
#include <array>
#include <fstream>
#include <sstream>
#include "utilities.h"
template <size_t rows, size_t cols, typename T>
struct Matrix;
struct uninitialized {
  constexpr uninitialized() noexcept = default;
};
inline constexpr uninitialized uninit{};
template <typename Matr, size_t r, size_t c, typename T>
struct baseMatr {
  template <typename Ma, size_t otherMatrixRows, size_t otherMatrixCols, typename U>
  friend struct baseMatr;
  static constexpr size_t rows{r};
  static constexpr size_t cols{c};
  baseMatr() {lof.fill(0);}
  operator Matrix<r, c, T>() const;
  std::ostream& print(std::ostream& os) const {
    const auto &self = static_cast<const Matr&>(*this);
    os << '[';
    for (size_t i{0}; i < rows; ++i) {
      os << '[';
      for (size_t z{0}; z < cols; ++z) {
        os << self[i, z];
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
  friend std::ostream& operator<<(std::ostream &os, const Matr &m) {
    return m.print(os);
  }
  protected:
    alignas(64) std::array<T, rows * cols> lof;
    explicit baseMatr(uninitialized){}
};
template <size_t rows, size_t cols, typename T>
struct Transposed : baseMatr<Transposed<rows, cols, T>, rows, cols, T> {
  using bm = baseMatr<Transposed, rows, cols, T>;
  const Matrix<cols, rows, T> &m;
  constexpr T& operator[](const size_t r, const size_t c) {
    return m[c, r];
  }
  const T& operator[](const size_t r, const size_t c) const {
    return m[c, r];
  }
  [[nodiscard]] Matrix<cols, rows, T> &depose() {
    return m;
  }
  [[nodiscard]] Matrix<cols, rows, T> &depose() const {
    return m;
  }
  explicit Transposed(const Matrix<cols, rows, T> &x) : m(x) {}
  [[nodiscard]] Matrix<rows, cols, T> eval() const {
    Matrix<rows, cols, T> result(uninit);
    for (size_t r{0}; r < rows; r++) {
      for (size_t c{0}; c < cols; c++) {
        result[r, c] = (*this)[r, c];
      }
    }
    return result;
  }
};

template <size_t rows, size_t cols, typename T>
struct Add : baseMatr<Add<rows, cols, T>, rows, cols, T> {
  using bm = baseMatr<Add, rows, cols, T>;
  const Matrix<rows, cols, T> &m;
  const Matrix<rows, cols, T> &x;
  constexpr T operator[](const size_t r, const size_t c) {
    return m[r, c] + x[r, c];
  }
  T operator[](const size_t r, const size_t c) const {
    return m[r, c] + x[r, c];
  }
  explicit Add(const Matrix<rows, cols, T> &m, const Matrix<rows, cols, T> &x) : m(m), x(x) {}
  [[nodiscard]] Matrix<rows, cols, T> eval() const {
    Matrix<rows, cols, T> result(uninit);
    for (size_t r{0}; r < rows; r++) {
      for (size_t c{0}; c < cols; c++) {
        result[r, c] = m[r, c] + x[r, c];
      }
    }
    return result;
  }
};

template <size_t rows, size_t cols, typename T>
struct Subt : baseMatr<Subt<rows, cols, T>, rows, cols, T> {
  using bm = baseMatr<Subt, rows, cols, T>;
  const Matrix<rows, cols, T> &m;
  const Matrix<rows, cols, T> &x;
  constexpr T operator[](const size_t r, const size_t c) {
    return m[r, c] - x[r, c];
  }
  T operator[](const size_t r, const size_t c) const {
    return m[r, c] - x[r, c];
  }
  explicit Subt(const Matrix<rows, cols, T> &m, const Matrix<rows, cols, T> &x) : m(m), x(x) {}
  [[nodiscard]] Matrix<rows, cols, T> eval() const {
    Matrix<rows, cols, T> result(uninit);
    for (size_t r{0}; r < rows; r++) {
      for (size_t c{0}; c < cols; c++) {
        result[r, c] = m[r, c] - x[r, c];
      }
    }
    return result;
  }
};

template <size_t rows, size_t cols, typename T>
struct Scale : baseMatr<Scale<rows, cols, T>, rows, cols, T> {
  using bm = baseMatr<Scale, rows, cols, T>;
  const Matrix<rows, cols, T> &m;
  const double i;
  explicit Scale(const Matrix<rows, cols, T> &m, const double i) : bm(uninit), m(m), i(i) {}
  explicit Scale(const double i, const Matrix<rows, cols, T> &m) : m(m), i(i) {}
  constexpr T operator[](const size_t r, const size_t c) {
    return (this->m)[r, c] * this->i;
  }
  T operator[](const size_t r, const size_t c) const {
    return (this->m)[r, c] * this->i;
  }
  [[nodiscard]] Matrix<rows, cols, T> eval() const {
    Matrix<rows, cols, T> result(uninit);
    for (size_t r{0}; r < rows; r++) {
      for (size_t c{0}; c < cols; c++) {
        result[r, c] = (this->m)[r, c] * this->i;
      }
    }
    return result;
  }
};

template <size_t rows, size_t cols, typename T>
struct Hada : baseMatr<Hada<rows, cols, T>, rows, cols, T> {
  using bm = baseMatr<Hada, rows, cols, T>;
  const Matrix<rows, cols, T> &m;
  const Matrix<rows, cols, T> &x;
  constexpr T operator[](const size_t r, const size_t c) {
    return (this->m)[r, c] * (this->x)[r, c];
  }
  T operator[](const size_t r, const size_t c) const {
    return (this->m)[r, c] * (this->x)[r, c];
  }
  explicit Hada(const Matrix<rows, cols, T> &m, const Matrix<rows, cols, T> &x) 
  : m(m), x(x) {}
  [[nodiscard]] Matrix<rows, cols, T> eval() const {
		Matrix<rows, cols, T> result(uninit);
    for (size_t r{0}; r < rows; r++) {
      for (size_t c{0}; c < cols; c++) {
        result[r, c] = (this->m)[r, c] * (this->x)[r, c];
      }
    }
    return result;
  }
};

template <size_t rows, size_t cols, typename T>
struct Bias : baseMatr<Bias<rows, cols, T>, rows, cols, T> {
	using bm = baseMatr<Bias, rows, cols, T>;
  const Matrix<rows, cols, T> &m;
  const Matrix<1, cols, T> &b;
	explicit Bias(const Matrix<rows, cols, T> &m, const Matrix<1, cols, T> &b)
    : m(m), b(b) {}
  constexpr T operator[](const size_t r, const size_t c) {
    return (this->m)[r, c] + (this->b)[0, c];
  }
  T operator[](const size_t r, const size_t c) const {
    return (this->m)[r, c] + (this->b)[0, c];
  }
  [[nodiscard]] Matrix<rows, cols, T> eval() const {
    Matrix<rows, cols, T> result(uninit);
    for (size_t r{0}; r < rows; r++) {
      for (size_t c{0}; c < cols; c++) {
        result[r, c] = (this->m)[r, c] + (this->b)[0, c];
      }
    }
    return result;
  }
};

template <size_t rows, size_t cols, typename T = double>
struct Matrix : baseMatr<Matrix<rows, cols, T>, rows, cols, T> {
  using bm = baseMatr<Matrix, rows, cols, T>;
  constexpr Matrix() : bm() {}
  constexpr Matrix(const Matrix &m) = default;
  constexpr Matrix(std::initializer_list<std::initializer_list<T>> list)
    : bm(uninit) {
      size_t r{0};
      for (const auto &row : list) {
        if (r >= rows) [[unlikely]] throw std::logic_error("too many values in rows");
        size_t c{0};
        for (const auto &val : row) {
          if (c >= cols) [[unlikely]] throw std::logic_error("too many values in cols");
          (*this)[r, c] = val;
          ++c;
        }
        ++r;
      }
    }
  constexpr T& operator[](const size_t r, const size_t c) {
    return this->lof[r * cols + c];
  }
  const T& operator[](const size_t r, const size_t c) const {
    return this->lof[r * cols + c];
  }
  [[nodiscard]] constexpr T& operator[](const size_t i) {
    return this->lof[i];
  }
  [[nodiscard]] constexpr Add<rows, cols, T> operator+(const Matrix &x) const {
    return Add<rows, cols, T>(*this, x);
  }
  [[nodiscard]] constexpr Hada<rows, cols, T> elementwise(const Matrix &x) const {
    return Hada<rows, cols, T>(*this, x);
  } 
  [[nodiscard]] constexpr Scale<rows, cols, T> operator*(const double i) const {
    return Scale<rows, cols, T>(*this, i);
  }
  [[nodiscard]] constexpr Transposed<cols, rows, T> transpose() const {
    return Transposed<cols, rows, T>(*this);
  }
  [[nodiscard]] constexpr Subt<rows, cols, T> operator-(const Matrix &m) const {
    return Subt<rows, cols, T>(*this, m);
  }
  template <typename Func>
  [[nodiscard]] constexpr Matrix elementwise(Func func) {
    Matrix result(uninit);
    for (size_t r{0}; r < rows; r++) {
      for (size_t c{0}; c < cols; c++) {
        result[r, c] = utilities::returnfunc(func, (*this)[r ,c]);
      }
    }
    return result;
  }
  explicit Matrix(uninitialized) : bm(uninit) {}
  template <typename C>
  requires std::derived_from<C, baseMatr<C, rows, cols, T>>
    && (!std::same_as<std::remove_cvref_t<C>, Matrix>)
  Matrix(const C &constructor)
    : bm(uninit) {
    for (size_t r{0}; r < rows; r++) {
      for (size_t c{0}; c < cols; c++) {
        (*this)[r, c] = constructor[r, c];
      }
    }
  }
  
  Bias<rows, cols, T> bias_add(Matrix<1, cols, T> &x) {
    return Bias<rows, cols, T>(*this, x);
  }
  [[nodiscard]] constexpr Matrix<1, cols, T> getrow(const size_t r) const {
    Matrix<1, cols, T> result(uninit);
    for (size_t c{0}; c < cols; c++) {
      result[0, c] = (*this)[r, c];
    }
    return result;
  }
  // for inverse of stock data
  [[nodiscard]] constexpr Matrix<2, 2, T> inv2x2() const {
      static_assert(rows == 2 && cols == 2, "2x2 inverse only");
      T a {(*this)[0, 0]};
      T b {(*this)[0, 1]};
      T c {(*this)[1, 0]};
      T d {(*this)[1, 1]};
      T det {(a * d) - (b * c)};
      if (det == static_cast<T>(0)) {
        std::cout << "o\n";
        return (*this);
      }
      T invDet {static_cast<T>(1) / det};
      Matrix<2, 2, T> retInv(uninit);
      retInv[0, 0] = d * invDet;
      retInv[0, 1] = -b * invDet;
      retInv[1, 0] = -c * invDet;
      retInv[1, 1] = a * invDet;
      return retInv;
  }
};
template <typename Lderived, typename Rderived, size_t lRows, 
         size_t shared, size_t rCols, typename T>
static Matrix<lRows, rCols, T> operator*(
const baseMatr<Lderived, lRows, shared, T> &leftM,
const baseMatr<Rderived, shared, rCols, T> &rightM) {
  const auto &L {static_cast<const Lderived &>(leftM)};
  const auto &R {static_cast<const Rderived &>(rightM)};
  Matrix<lRows, rCols, T> fin(uninit);
  T runningTotal {static_cast<T>(0)};
  for (size_t r{0}; r < lRows; r++) {
    for (size_t c{0}; c < rCols; c++) {
      for (size_t ind{0}; ind < shared; ind++) {
        runningTotal += L[r, ind] * R[ind, c];
      }
      fin[r, c] = runningTotal;
      runningTotal = static_cast<T>(0);
    }
  }
  return fin;
}
template <typename Derived, size_t r, size_t c, typename T>
baseMatr<Derived, r, c, T>::operator Matrix<r, c, T>() const {
  return static_cast<const Derived&>(*this).eval();
}

struct StockInfo {
  std::string date;
  double openPrice=0;
  double highPrice=0;
  double lowPrice=0;
  double closePrice=0;
  size_t volume=0;
  StockInfo() = default;
  StockInfo(const double o, const double h, const double l, const double c, const size_t v)
    : openPrice(o), highPrice(h), lowPrice(l), closePrice(c), volume(v) {}
  friend std::ostream& operator<<(std::ostream &os, const StockInfo &stockA) {
    os << std::fixed << std::setprecision(2);
    os << "Open $" << stockA.openPrice << " | Close $" << stockA.closePrice;
    return os;
  }
  friend std::ostream& operator<<(std::ostream &os, const std::vector<StockInfo> &stockA) {
    for (const auto &row : stockA) {
      os << row << '\n';
    }
    return os;
  }
};

std::vector<StockInfo> loadcsv(const std::string &filename) {
  std::vector<StockInfo> dataset;
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "error could not open file " << filename << '\n';
    return dataset;
  }
  std::string line;
  std::getline(file, line);
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string date, openPrice, highPrice, lowPrice, closePrice, volume;
    std::getline(ss, date, ',');
    std::getline(ss, openPrice, ',');
    std::getline(ss, highPrice, ',');
    std::getline(ss, lowPrice, ',');
    std::getline(ss, closePrice, ',');
    std::getline(ss, volume, ',');
    StockInfo stock;
    stock.date = date;
    stock.openPrice = std::stod(openPrice);
    stock.highPrice = std::stod(highPrice);
    stock.lowPrice = std::stod(lowPrice);
    stock.closePrice = std::stod(closePrice);
    stock.volume = std::stol(volume);
    dataset.push_back(stock);
  }
  file.close();
  return dataset;
}

template <size_t rows>
Matrix<rows, 2, double> stockMatr(const std::vector<StockInfo> &stockData) {
  Matrix<rows, 2, double> fin;
  if (!stockData.empty()) {
    fin[0, 0] = 0.0;
    fin[0, 1] = static_cast<double>(stockData[0].volume);
  }
  for (size_t r{1}; r < rows && r < stockData.size(); r++) {
    double prev = stockData[r - 1].closePrice;
    double curr = stockData[r].closePrice;
    fin[r, 0] = (prev != 0.0) ? (curr - prev) / prev : 0.0;
    fin[r, 1] = static_cast<double>(stockData[r].volume);
  }
  return fin;
}
template <size_t rows>
double meanReturn(const Matrix<rows, 2> &stockData) {
  double fin {0.0};
  for (size_t i{1}; i < rows; i++) {
    fin += stockData[i, 0];
  }
  return fin / static_cast<double>(rows - 1);
}
template <size_t rows>
double standardDev(const Matrix<rows, 2> &stockData) {
  const double fancyU = meanReturn(stockData);
  double total {0};
  for (size_t i{1}; i < rows; i++) {
    total += (stockData[i, 0] - fancyU) * (stockData[i, 0] - fancyU);
  }
  total /= static_cast<double>(rows - 1.0);
  return std::sqrt(total);
}

double sigmoid(double x) {
  return 1.0 / (1.0 + std::exp(-x));
}

struct aiFuncs {
  template <size_t rows>
  static std::vector<double> emaRatios(const Matrix<rows, 2> &data, size_t days) {
    std::vector<double> fin(rows, 0.0);
    double alpha = 2 / (static_cast<double>(days) + 1);
    double currEma = data[0, 0];
    for (size_t i{0}; i < rows; i++) {
      double currPrice = data[i, 0];
      currEma = (currPrice * alpha) + (currEma * (1.0 - alpha));
      if (currEma > 0.0) {
        fin[i] = (currPrice - currEma) / currEma;
      }
    }
    return fin;
  }
  template <size_t samples, size_t cols, size_t Nplus1>
  static void buildFeatMatr(const Matrix<Nplus1, 2> &stockTest, 
                     size_t lookback,
                     Matrix<samples, cols> &X,
                     Matrix<samples, 1> &Y) 
  {
    std::vector<double> emaRats = emaRatios(stockTest, 5);
    for (size_t i{0}; i < samples; i++) {
      size_t idx = i + lookback;
      double retT0 = stockTest[idx, 0];
      double retT1 = stockTest[idx - 1, 0];
      double ret5D {1.0};
      for (size_t k{0}; k < 5; k++) ret5D *= (1.0 + stockTest[idx - k, 0]);
      ret5D -= 1.0;
      double avgVol {0.0};
      for (size_t k{0}; k < 5; k++) avgVol += stockTest[idx - k, 1];
      double volRatio = (avgVol > 0.0) ? (stockTest[idx, 1] / (avgVol / 5.0)) : 1.0;
      X[i, 0] = 1.0;
      X[i, 1] = retT0;
      X[i, 2] = retT1;
      X[i, 3] = ret5D;
      X[i, 4] = volRatio;
      X[i, 5] = emaRats[idx];
      Y[i, 0] = stockTest[idx + 1, 0];
    }
  }
  // again, cols = features + 1
  template <size_t sampleSize, size_t cols>
  static void normalize(Matrix<sampleSize, cols> &X, 
                 std::vector<double> &means, 
                 std::vector<double> &stdDevs) 
  {
    means.resize(cols, 0.0);
    stdDevs.resize(cols, 1.0);
    for (size_t c{0}; c < cols; c++) {
      double cMean = 0.0;
      for (size_t i{0}; i < sampleSize; i++) cMean += X[i, c];
      cMean /= static_cast<double>(sampleSize);
      double cDev = 0.0;
      for (size_t i{0}; i < sampleSize; i++) {
        cDev += (X[i, c] - cMean) * (X[i, c] - cMean);
      }
      double cStdDev = std::sqrt(cDev / static_cast<double>(sampleSize));
      means[c] = cMean;
      stdDevs[c] = cStdDev;
      constexpr double epsilon = 1e-9;
      if (std::abs(cStdDev) > epsilon) {
        for (size_t i{0}; i < sampleSize; i++) {
          X[i, c] = (X[i, c] - cMean) / cStdDev;
        }
      }
    }
  }
  // cols is feat + 1
  template <size_t sampleSize, size_t cols>
  static Matrix<cols, 1> findRegression(const Matrix<sampleSize, cols> &X,
                                     const Matrix<sampleSize, 1> &Y,
                                     double learnRate,
                                     size_t epochs)
  {
    Matrix<cols, 1> beta;
    for (size_t epoch{0}; epoch < epochs; epoch++) {
      Matrix<sampleSize, 1> err = (X * beta) - Y;
      Matrix<cols, 1> grad = (X.transpose() * err) * (1.0 / static_cast<double>(sampleSize));
      beta = beta - (grad * learnRate);
    }
    return beta;
  }
  template <size_t sampleSize, size_t cols>
  static void getPerformance(const Matrix<sampleSize, cols> &X,
                             const Matrix<sampleSize, 1> &Y,
                             const Matrix<cols, 1> &beta)
  {
    Matrix<sampleSize, 1> preds = X * beta;
    double totalMseErr = 0.0;
    size_t totalDirAcc = 0;
    for (size_t i{0}; i < sampleSize; i++) {
      double pred = preds[i, 0];
      double act  = Y[i, 0];
      double tempErr = act - pred;
      totalMseErr += tempErr * tempErr;
      if ((pred >= 0.0 && act >= 0.0) || (pred < 0.0 && act < 0.0)) {
        ++totalDirAcc;
      }
    }
    double finMse = totalMseErr / static_cast<double>(sampleSize);
    double rMse = std::sqrt(finMse);
    double finDirAcc = (static_cast<double>(totalDirAcc) / static_cast<double>(sampleSize)) * 100;
    std::cout << "\n--- MODEL ACCURACY ---\n"
              << "Mean Squared Error   : " << finMse << '\n'
              << "Root Mean Sq. Error  : " << rMse << '\n'
              << "Directional Accuracy : " << finDirAcc << "%\n";
  }
  template <size_t fullRows, size_t subRows, size_t cols>
  static Matrix<subRows, cols> sliceData(const Matrix<fullRows, cols> &m, size_t start) {
    Matrix<subRows, cols> fin(uninit);
    for (size_t r{0}; r < subRows; r++) {
      for (size_t c{0}; c < cols; c++) {
        fin[r, c] = m[start + r, c];
      }
    }
    return fin;
  }
  template <size_t sampleSize, size_t cols>
  static void applyNormalize(Matrix<sampleSize, cols> &X, 
                             std::vector<double> &means,
                             std::vector<double> &stdDevs)
  {
    constexpr double epsilon = 1e-9;
    for (size_t c{0}; c < cols; c++) {
      if (std::abs(stdDevs[c]) > epsilon) {
        for (size_t i{0}; i < sampleSize; i++) {
          X[i, c] = (X[i, c] - means[c]) / stdDevs[c];
        }
      }
    }
  }
  template <size_t cols>
  static void mxb(const Matrix<cols, 1> &beta,
                  const Matrix<1, cols> &normalizedRow,
                  int featInd=-1) {
    double x {0.0};
    double b = beta[0, 0];
    if (featInd == -1) {
      for (size_t c{1}; c < cols; c++) {
        x += normalizedRow[0, c] * beta[c, 0];
      }
      double m = 1.0;
      double y = (m * x) + b;
      std::cout << "Final Equation: y = " << m << "x + " << b
                << "\nNext Prediction: " << y << '\n';
    } else {
      size_t c = static_cast<size_t>(featInd) + 1;
      if (c >= cols) [[unlikely]] {
        std::cerr << "Error: feat index out of range\n";
        return;
      }
      double m = normalizedRow[featInd, 0];
      for (size_t c{1}; c < cols; c++) x += normalizedRow[0, c] * beta[c, 0];
      double y = (m * x) + b;
      std::cout << "Final Equation: y = " << m << "x + " << b
        << "\nNext Prediction: " << std::fixed << std::setprecision(12) << m * x
        << std::setprecision(6) << '\n';
    }
  }
};

int main() {
  using namespace std;
  using namespace utilities;
  using ai = aiFuncs;
  vector<StockInfo> csvTest = loadcsv("/Users/jonah/Downloads/aapl_prices.csv");
  constexpr size_t N = 1234;
  constexpr size_t lookback = 5;
  constexpr size_t sampleSize = N - lookback - 1;
  constexpr size_t feat = 5;
  Matrix<N + 1, 2> stockTest = stockMatr<N + 1>(csvTest);
  Matrix<sampleSize, feat + 1> X;
  Matrix<sampleSize, 1> Y;
  ai::buildFeatMatr(stockTest, lookback, X, Y);
  constexpr size_t learnSize = static_cast<size_t>(sampleSize * 0.8);
  constexpr size_t testSize  = sampleSize - learnSize;
  Matrix<learnSize, feat+1> XTrain = ai::sliceData<sampleSize, learnSize>(X, 0);
  Matrix<learnSize, 1> YTrain = ai::sliceData<sampleSize, learnSize>(Y, 0);
  Matrix<testSize, feat+1> XTest = ai::sliceData<sampleSize, testSize>(X, learnSize);
  Matrix<testSize, 1> YTest = ai::sliceData<sampleSize, testSize>(Y, learnSize);
  vector<double> featMeans, featStdDevs;
  ai::normalize(XTrain, featMeans, featStdDevs);
  ai::applyNormalize(XTest, featMeans, featStdDevs);
  double learnRate = 0.01;
  size_t epochs = 824;
  Matrix<feat + 1, 1> beta = ai::findRegression(XTrain, YTrain, learnRate, epochs);
  cout << fixed << setprecision(6);
  cout << "==== TRAINED WEIGHTS - BETA ====\n"
    << "Intercept        : " << beta[0, 0] << '\n'
    << "Return Today     : " << beta[1, 0] << '\n'
    << "Return Yesterday : " << beta[2, 0] << '\n'
    << "5-Day Trend      : " << beta[3, 0] << '\n'
    << "Vol Ratio        : " << beta[4, 0] << '\n'
    << "5-Day EMA Ratio  : " << beta[5, 0] << '\n';
  cout << "\n==== IN-SAMPLE PERFORMANCE ====\n";
  ai::getPerformance(XTrain, YTrain, beta);
  cout << "\n==== OUT OF SAMPLE PERFORMANCE ====\n";
  Matrix<1, feat+1> XLast;
  for (size_t i{0}; i < feat; i++) {
    XLast[0, i] = XTest[testSize, i];
  }
  ai::getPerformance(XTest, YTest, beta);
  ai::mxb(beta, XLast);
  return 0;
}
