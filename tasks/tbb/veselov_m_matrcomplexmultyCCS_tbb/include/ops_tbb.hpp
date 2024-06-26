// Copyright 2023 Veselov Mikhail
#pragma once

#include <vector>

#include "core/task/include/task.hpp"

namespace VeselovTbb {
struct Complex {
  double real;
  double imag;
};

class SparseMatrixComplexMultiTBBSequential : public ppc::core::Task {
 public:
  explicit SparseMatrixComplexMultiTBBSequential(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<Complex> val1{};
  std::vector<int> rows1{};
  std::vector<int> cols1{};
  int numRows1{};
  int numCols1{};
  std::vector<Complex> val2{};
  std::vector<int> rows2{};
  std::vector<int> cols2{};
  int numRows2{};
  int numCols2{};
  Complex* res{};
};

class SparseMatrixComplexMultiTBBParallel : public ppc::core::Task {
 public:
  explicit SparseMatrixComplexMultiTBBParallel(std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<Complex> val1{};
  std::vector<int> rows1{};
  std::vector<int> cols1{};
  int numRows1{};
  int numCols1{};
  std::vector<Complex> val2{};
  std::vector<int> rows2{};
  std::vector<int> cols2{};
  int numRows2{};
  int numCols2{};
  Complex* res{};
};
}  // namespace VeselovTbb