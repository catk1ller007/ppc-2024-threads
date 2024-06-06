// Copyright 2024 Kulaev Zhenya
#pragma once

#include <random>
#include <vector>

#include "core/task/include/task.hpp"

namespace kulaev_e_block_stl {

class TestSTLSequentialKulaevCannon : public ppc::core::Task {
 public:
  explicit TestSTLSequentialKulaevCannon(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<double> A;
  std::vector<double> B;
  std::vector<double> result;
  int n = 0, m = 0;
};

std::vector<double> getRandomMatrix(int rows, int cols);

std::vector<double> multiplyMatrix(const std::vector<double>& A, const std::vector<double>& B, int rows_A, int col_B);

class TestTaskSTLParallelKulaevCannon : public ppc::core::Task {
 public:
  explicit TestTaskSTLParallelKulaevCannon(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<double> A;
  std::vector<double> B;
  std::vector<double> result;
  int n = 0, m = 0;
};

std::vector<double> cannonMatrixMultiplication_stl(const std::vector<double>& A, const std::vector<double>& B, int n,
                                                   int m);

std::vector<double> cannonMatrixMultiplication(const std::vector<double>& A, const std::vector<double>& B, int n,
                                               int m);
}  // namespace kulaev_e_block_stl