// Copyright 2024 Sadikov Damir
#include <gtest/gtest.h>
#include <tbb/tbb.h>

#include <chrono>
#include <complex>
#include <random>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "tbb/sadikov_d_crs_mult/include/sadikov_d_tbb.hpp"

matrix_CRS generate_random_matrix(int n, int m, int seed, double p) {
  std::mt19937 gen;
  gen.seed(seed);
  std::uniform_real_distribution<double> rnd(-2.0, 2.0);
  std::bernoulli_distribution bd(p);

  matrix_CRS res;
  res.n = n;
  res.m = m;
  res.row_id.assign(res.n + 1, 0);
  std::vector<std::vector<std::pair<int, std::complex<double>>>> temp(res.n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      if (bd(gen)) {
        std::complex<double> val(rnd(gen), rnd(gen));
        temp[i].emplace_back(j, val);
      }
    }
  }
  for (int i = 0; i < res.n; i++) {
    res.row_id[i + 1] = res.row_id[i];
    for (auto &j : temp[i]) {
      res.col.push_back(j.first);
      res.value.push_back(j.second);
      res.row_id[i + 1]++;
    }
  }

  return res;
}

TEST(sadikov_d_crs_mult_tbb, test_pipeline_run) {
  matrix_CRS A = generate_random_matrix(123, 123, 2024, 0.75);
  matrix_CRS B = generate_random_matrix(123, 123, 3003, 0.75);
  matrix_CRS C;

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataTbb = std::make_shared<ppc::core::TaskData>();
  taskDataTbb->inputs.emplace_back(reinterpret_cast<uint8_t *>(&A));
  taskDataTbb->inputs.emplace_back(reinterpret_cast<uint8_t *>(&B));
  taskDataTbb->outputs.emplace_back(reinterpret_cast<uint8_t *>(&C));

  // Create Task
  auto taskTbb = std::make_shared<CRSComplexMult_tbb>(taskDataTbb);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 50;
  const auto t0 = oneapi::tbb::tick_count::now();
  perfAttr->current_timer = [&] { return (oneapi::tbb::tick_count::now() - t0).seconds(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(taskTbb);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
}

TEST(sadikov_d_crs_mult_tbb, test_task_run) {
  matrix_CRS A = generate_random_matrix(123, 123, 2024, 0.75);
  matrix_CRS B = generate_random_matrix(123, 123, 3003, 0.75);
  matrix_CRS C;

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataTbb = std::make_shared<ppc::core::TaskData>();
  taskDataTbb->inputs.emplace_back(reinterpret_cast<uint8_t *>(&A));
  taskDataTbb->inputs.emplace_back(reinterpret_cast<uint8_t *>(&B));
  taskDataTbb->outputs.emplace_back(reinterpret_cast<uint8_t *>(&C));

  // Create Task
  auto taskTbb = std::make_shared<CRSComplexMult_tbb>(taskDataTbb);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 50;
  const auto t0 = oneapi::tbb::tick_count::now();
  perfAttr->current_timer = [&] { return (oneapi::tbb::tick_count::now() - t0).seconds(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(taskTbb);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
}
