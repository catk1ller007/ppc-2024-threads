// Copyright 2024 Kosarev Egor
#pragma once

#include <algorithm>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "core/task/include/task.hpp"

struct Point {
  int x, y;

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }

  bool operator<(const Point& other) const {
    return x < other.x || (x == other.x && y < other.y);
  }
};

class TestTaskSequentialKosarevJarvisHull : public ppc::core::Task {
 public:
  explicit TestTaskSequentialKosarevJarvisHull(
      std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<Point> points;
  std::vector<Point> pointsHull;
};

class TestOMPTaskParallelKosarevJarvisHull : public ppc::core::Task {
 public:
  explicit TestOMPTaskParallelKosarevJarvisHull(
      std::shared_ptr<ppc::core::TaskData> taskData_)
      : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<Point> points;
  std::vector<Point> pointsHull;
};

int orientation(Point p, Point q, Point r);

double distance(Point p1, Point p2);

std::vector<Point> JarvisAlgo(std::vector<Point>& arrPoints);