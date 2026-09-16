#include <algorithm>
#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <functional>
#include <print>
#include <random>

template <std::invocable<> F>
std::chrono::microseconds measure_microseconds(F&& f) {
  auto start = std::chrono::high_resolution_clock::now();
  f();
  auto end = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

template <std::invocable<> F, std::invocable<> R>
std::chrono::microseconds
measure_average_microseconds(F&& f, R&& restart, std::size_t runs) {
  std::chrono::microseconds total{};

  for (std::size_t run = 0; run < runs; run++) {
    restart();
    total += measure_microseconds(std::forward<F>(f));
  }

  return total / runs;
}

template <std::size_t N>
using Matrix = std::array<std::array<double, N>, N>;

template <std::size_t N>
void fill_random(Matrix<N>& matrix, std::mt19937& rng) {
  std::uniform_int_distribution dist{0, 10};

  std::ranges::for_each(matrix, [&](auto& row) {
    std::ranges::generate(row, [&] { return dist(rng); });
  });
}

template <std::size_t N>
void clear(Matrix<N>& matrix) {
  std::ranges::for_each(matrix, [](auto& row) { std::ranges::fill(row, 0.0); });
}

template <std::size_t N>
double checksum(const Matrix<N>& matrix) {
  double result = 0.0;

  for (const auto& row : matrix)
    result = std::ranges::fold_left(row, result, std::plus{});

  return result;
}

template <std::size_t N>
void multiply(const Matrix<N>& A, const Matrix<N>& B, Matrix<N>& C) {
  for (std::size_t i = 0; i < N; i++)
    for (std::size_t j = 0; j < N; j++)
      for (std::size_t k = 0; k < N; k++)
        C[i][j] += A[i][k] * B[k][j];
}

template <std::size_t N>
void test(std::size_t runs) {
  Matrix<N> A;
  Matrix<N> B;
  Matrix<N> C;

  std::mt19937 rng{std::invoke(std::random_device{})};

  fill_random(A, rng);
  fill_random(B, rng);
  clear(C);

  double result_checksum = 0.0;

  auto time = measure_average_microseconds(
    [&] { multiply(A, B, C); },
    [&] {
      result_checksum += checksum(C);
      clear(C);
    },
    runs
  );

  result_checksum += checksum(C);

  std::println("{:<10}|{:<20}|{:<20}", N, time.count(), result_checksum);
}

int main() {
  std::println("{:<10}|{:<20}|{:<20}", "N", "Tiempo promedio (μs)", "Checksum");

  test<64>(20);
  test<128>(10);
  test<256>(5);
  test<512>(3);
}
