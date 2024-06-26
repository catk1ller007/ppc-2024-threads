// Copyright 2024 Sokolova Daria
#include <gtest/gtest.h>

#include <vector>

#include "core/perf/include/perf.hpp"
#include "omp/sokolova_d_linear_image_filtering/include/ops_omp.hpp"

using namespace sokolova_omp;

TEST(sokolova_d_linear_image_filtering_omp, test_pipeline_run) {
  uint32_t width = 1024;
  uint32_t height = 1024;

  // Create data
  std::vector<uint8_t> in(width * height * 3, 255);
  std::vector<uint8_t> out(width * height * 3, 0);
  std::vector<uint8_t> expected(width * height * 3, 255);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataOmp = std::make_shared<ppc::core::TaskData>();
  taskDataOmp->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskDataOmp->inputs_count.emplace_back(width);
  taskDataOmp->inputs_count.emplace_back(height);
  taskDataOmp->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskDataOmp->outputs_count.emplace_back(width);
  taskDataOmp->outputs_count.emplace_back(height);

  // Create Task
  auto gaussFilterHorizontalOmp = std::make_shared<GaussFilterHorizontalOmp>(taskDataOmp);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(gaussFilterHorizontalOmp);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_EQ(expected, out);
}

TEST(sokolova_d_linear_image_filtering_omp, test_task_run) {
  uint32_t width = 1024;
  uint32_t height = 1024;

  // Create data
  std::vector<uint8_t> in(width * height * 3, 255);
  std::vector<uint8_t> out(width * height * 3, 0);
  std::vector<uint8_t> expected(width * height * 3, 255);

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataOmp = std::make_shared<ppc::core::TaskData>();
  taskDataOmp->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  taskDataOmp->inputs_count.emplace_back(width);
  taskDataOmp->inputs_count.emplace_back(height);
  taskDataOmp->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  taskDataOmp->outputs_count.emplace_back(width);
  taskDataOmp->outputs_count.emplace_back(height);

  // Create Task
  auto gaussFilterHorizontalOmp = std::make_shared<GaussFilterHorizontalOmp>(taskDataOmp);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(gaussFilterHorizontalOmp);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  ASSERT_EQ(expected, out);
}
