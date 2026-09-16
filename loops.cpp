#include <algorithm>
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
    total += measure_microseconds(std::forward<F>(f));
    restart();
  }
  return total / runs;
}

template <std::size_t N>
void test() {
  double A[N][N], x[N], y[N] = {};

  static std::mt19937 rng{std::invoke(std::random_device{})};
  std::uniform_int_distribution dist{0, 100};

  std::ranges::for_each(A, [&](auto&& row) {
    std::ranges::generate(row, [&] { return dist(rng); });
  });
  std::ranges::generate(x, [&] { return dist(rng); });

  double row_checksum = 0.0;

  // This is O(n^2)
  auto row_microseconds = measure_average_microseconds(
    [&] {
      for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
          y[i] += A[i][j] * x[j];
    },
    [&] {
      row_checksum += std::ranges::fold_left(y, 0.0, std::plus{});
      std::ranges::fill(y, 0.0);
    },
    100
  );

  double col_checksum = 0.0;

  // This is O(n^2) too
  auto col_microsecond = measure_average_microseconds(
    [&] {
      for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
          y[i] += A[i][j] * x[j];
    },
    [&] {
      col_checksum += std::ranges::fold_left(y, 0.0, std::plus{});
      std::ranges::fill(y, 0.0);
    },
    100
  );

  std::println(
    "{:<15}|{:<15}|{:<15}|{:<15}|{:<15}",
    N,
    row_microseconds.count(),
    col_microsecond.count(),
    row_checksum,
    col_checksum
  );
}

template <std::size_t N>
  requires(N != 0)
void multi_test() {
  test<N>();
  if constexpr (requires { multi_test<N - 1>(); }) {
    multi_test<N - 1>();
  }
}

int main() {
  std::println(
    "{:<15}|{:<15}|{:<15}|{:<15}|{:<15}",
    "Tamaño",
    "T. filas(μs)",
    "T. columnas(μs)",
    "Checksum F.",
    "Checksum C."
  );
  multi_test<512>();
}
