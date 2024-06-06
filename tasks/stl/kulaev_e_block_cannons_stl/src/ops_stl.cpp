// Copyright 2024 Kulaev Zhenya
#include "stl/kulaev_e_block_cannons_stl/include/ops_stl.hpp"

#include <algorithm>
#include <thread>
#include <vector>

namespace kulaev_e_block_stl {

std::vector<double> cannonMatrixMultiplication(const std::vector<double>& A, const std::vector<double>& B, int n,
                                               int m) {
  int blockSize = std::min(n, m);

  std::vector<double> C(n * m, 0.0);

  if (n == 0 || m == 0) {
    return std::vector<double>();
  }

  for (int i = 0; i < n; i += blockSize) {
    for (int j = 0; j < m; j += blockSize) {
      for (int k = 0; k < m; k += blockSize) {
        int i_end = std::min(i + blockSize, n);
        int j_end = std::min(j + blockSize, m);
        int k_end = std::min(k + blockSize, m);

        for (int ii = i; ii < i_end; ++ii) {
          for (int kk = k; kk < k_end; ++kk) {
            double A_ik = A[ii * m + kk];
            for (int jj = j; jj < j_end; ++jj) {
              C[ii * m + jj] += A_ik * B[kk * m + jj];
            }
          }
        }
      }
    }
  }

  return C;
}

std::vector<double> multiplyMatrix(const std::vector<double>& A, const std::vector<double>& B, int rows_A, int col_B) {
  int col_A = rows_A;
  std::vector<double> C(rows_A * col_B, 0.0);

  if (rows_A == 0 || col_B == 0) {
    return std::vector<double>();
  }

  for (int i = 0; i < rows_A; ++i) {
    for (int j = 0; j < col_B; ++j) {
      for (int k = 0; k < col_A; ++k) {
        C[i * col_B + j] += A[i * col_A + k] * B[k * col_B + j];
      }
    }
  }
  return C;
}

void multiplyBlocks(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C, int n, int m,
                    int blockSize, int iStart, int jStart, int kStart) {
  for (int i = iStart; i < std::min(iStart + blockSize, n); ++i) {
    for (int j = jStart; j < std::min(jStart + blockSize, m); ++j) {
      for (int k = kStart; k < std::min(kStart + blockSize, m); ++k) {
        C[i * m + j] += A[i * m + k] * B[k * m + j];
      }
    }
  }
}

std::vector<double> cannonMatrixMultiplication_stl(const std::vector<double>& A, const std::vector<double>& B, int n,
                                                   int m) {
  int blockSize = std::min(n, m);
  std::vector<double> C(n * m, 0.0);

  if (n == 0 || m == 0) {
    return std::vector<double>();
  }

  int countThreads = std::thread::hardware_concurrency();
  if (countThreads == 0) {
    countThreads = 1;
  }

  std::vector<std::thread> threads(countThreads);
  int blockSizePerThread = (n + countThreads - 1) / countThreads;

  for (int t = 0; t < countThreads; ++t) {
    int startRow = t * blockSizePerThread;
    int endRow = std::min((t + 1) * blockSizePerThread, n);

    threads[t] = std::thread([&, startRow, endRow] {
      std::vector<double> local_C(n * m, 0.0);

      for (int i = startRow; i < endRow; i += blockSize) {
        for (int j = 0; j < m; j += blockSize) {
          for (int k = 0; k < m; k += blockSize) {
            multiplyBlocks(A, B, local_C, n, m, blockSize, i, j, k);
          }
        }
      }

      for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < m; ++j) {
          C[i * m + j] += local_C[i * m + j];
        }
      }
    });
  }

  for (auto& thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  return C;
}

std::vector<double> getRandomMatrix(int rows, int cols) {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::uniform_real_distribution<double> dis(1.0, 20.0);

  std::vector<double> matrix(rows * cols);

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      matrix[i * cols + j] = dis(gen);
    }
  }

  return matrix;
}

bool TestSTLSequentialKulaevCannon::pre_processing() {
  internal_order_test();
  // Init value for input and output

  A = std::vector<double>(taskData->inputs_count[0]);
  B = std::vector<double>(taskData->inputs_count[1]);

  n = *reinterpret_cast<int*>(taskData->inputs[2]);
  m = *reinterpret_cast<int*>(taskData->inputs[3]);

  auto* tmp_ptr_A = reinterpret_cast<double*>(taskData->inputs[0]);
  for (size_t i = 0; i < taskData->inputs_count[0]; i++) {
    A[i] = tmp_ptr_A[i];
  }

  auto* tmp_ptr_B = reinterpret_cast<double*>(taskData->inputs[1]);
  for (size_t i = 0; i < taskData->inputs_count[1]; i++) {
    B[i] = tmp_ptr_B[i];
  }
  return true;
}

bool TestSTLSequentialKulaevCannon::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] == taskData->inputs_count[1] &&
         taskData->inputs_count[0] == taskData->outputs_count[0] &&
         taskData->inputs_count[1] == taskData->outputs_count[0];
}

bool TestSTLSequentialKulaevCannon::run() {
  internal_order_test();
  result = cannonMatrixMultiplication(A, B, n, m);
  return true;
}

bool TestSTLSequentialKulaevCannon::post_processing() {
  internal_order_test();
  std::copy(result.begin(), result.end(), reinterpret_cast<double*>(taskData->outputs[0]));
  return true;
}

bool TestTaskSTLParallelKulaevCannon::pre_processing() {
  internal_order_test();
  // Init vectors
  A = std::vector<double>(taskData->inputs_count[0]);
  B = std::vector<double>(taskData->inputs_count[1]);

  n = *reinterpret_cast<int*>(taskData->inputs[2]);
  m = *reinterpret_cast<int*>(taskData->inputs[3]);

  auto* tmp_ptr_A = reinterpret_cast<double*>(taskData->inputs[0]);
  for (size_t i = 0; i < taskData->inputs_count[0]; i++) {
    A[i] = tmp_ptr_A[i];
  }

  auto* tmp_ptr_B = reinterpret_cast<double*>(taskData->inputs[1]);
  for (size_t i = 0; i < taskData->inputs_count[1]; i++) {
    B[i] = tmp_ptr_B[i];
  }
  return true;
}

bool TestTaskSTLParallelKulaevCannon::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] == taskData->inputs_count[1] &&
         taskData->inputs_count[0] == taskData->outputs_count[0] &&
         taskData->inputs_count[1] == taskData->outputs_count[0];
}

bool TestTaskSTLParallelKulaevCannon::run() {
  internal_order_test();
  result = cannonMatrixMultiplication_stl(A, B, n, m);
  return true;
}

bool TestTaskSTLParallelKulaevCannon::post_processing() {
  internal_order_test();
  std::copy(result.begin(), result.end(), reinterpret_cast<double*>(taskData->outputs[0]));
  return true;
}
}  // namespace kulaev_e_block_stl