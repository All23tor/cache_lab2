#include <algorithm>
#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <functional>
#include <print>
#include <random>
#include <ranges>

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
void multiply(
  const Matrix<N>& A,
  const Matrix<N>& B,
  Matrix<N>& C,
  std::size_t block_size
) {
  for (std::size_t ii = 0; ii < N; ii += block_size)
    for (std::size_t kk = 0; kk < N; kk += block_size)
      for (std::size_t jj = 0; jj < N; jj += block_size)

        for (std::size_t i = ii; i < std::min(ii + block_size, N); i++)
          for (std::size_t k = kk; k < std::min(kk + block_size, N); k++)
            for (std::size_t j = jj; j < std::min(jj + block_size, N); j++)
              C[i][j] += A[i][k] * B[k][j];
}

template <std::size_t N>
void test(std::size_t block_size, std::size_t runs) {
  Matrix<N> A;
  Matrix<N> B;
  Matrix<N> C;

  std::mt19937 rng{std::invoke(std::random_device{})};

  fill_random(A, rng);
  fill_random(B, rng);
  clear(C);

  double result_checksum = 0.0;

  auto time = measure_average_microseconds(
    [&] { multiply(A, B, C, block_size); },
    [&] {
      result_checksum += checksum(C);
      clear(C);
    },
    runs
  );

  result_checksum += checksum(C);

  std::println(
    "{:<10}|{:<10}|{:<20}|{:<20}",
    N,
    block_size,
    time.count(),
    result_checksum
  );
}

int main() {
  std::println(
    "{:<10}|{:<10}|{:<20}|{:<20}",
    "N",
    "Bloque",
    "Tiempo promedio (μs)",
    "Checksum"
  );

  for (std::size_t block_size : {8uz, 16uz, 32uz, 64uz}) {
    test<64>(block_size, 20);
    test<128>(block_size, 10);
    test<256>(block_size, 5);
    test<512>(block_size, 3);
  }
}
