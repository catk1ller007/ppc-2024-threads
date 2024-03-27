// Copyright 2024 Kosarev Egor
#include <gtest/gtest.h>
#include <omp.h>

#include <vector>

#include "core/perf/include/perf.hpp"
#include "omp/kosarev_e_jarvis_hull/include/ops_omp.hpp"

TEST(kosarev_e_jarvis_hull_omp, test_pipeline_run) {
  std::vector<Point> points = {{0, 3}, {1, 1}, {2, 2}, {4, 4}, {0, 0}, {1, 2}, {3, 1}, {3, 3}};
  std::vector<Point> hull = {{0, 3}, {4, 4}, {3, 1}, {0, 0}};
  std::vector<Point> resHull_par(points.size());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(points.data()));
  taskDataSeq->inputs_count.emplace_back(points.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(resHull_par.data()));
  taskDataSeq->outputs_count.emplace_back(resHull_par.size());

  // Create Task
  auto testTaskOMP = std::make_shared<TestOMPTaskParallelKosarevJarvisHull>(taskDataSeq);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  perfAttr->current_timer = [&] { return omp_get_wtime(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testTaskOMP);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  for (size_t i = 0; i < hull.size(); ++i) {
    ASSERT_EQ(resHull_par[i], hull[i]);
  }
}

TEST(kosarev_e_jarvis_hull_omp, test_task_run) {
  std::vector<Point> points = {{0, 3}, {1, 1}, {2, 2}, {4, 4}, {0, 0}, {1, 2}, {3, 1}, {3, 3}};
  std::vector<Point> hull = {{0, 3}, {4, 4}, {3, 1}, {0, 0}};
  std::vector<Point> resHull_par(points.size());

  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
  taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t *>(points.data()));
  taskDataSeq->inputs_count.emplace_back(points.size());
  taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t *>(resHull_par.data()));
  taskDataSeq->outputs_count.emplace_back(resHull_par.size());

  // Create Task
  auto testTaskOMP = std::make_shared<TestOMPTaskParallelKosarevJarvisHull>(taskDataSeq);

  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  perfAttr->current_timer = [&] { return omp_get_wtime(); };

  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testTaskOMP);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
  for (size_t i = 0; i < hull.size(); ++i) {
    ASSERT_EQ(resHull_par[i], hull[i]);
  }
}
