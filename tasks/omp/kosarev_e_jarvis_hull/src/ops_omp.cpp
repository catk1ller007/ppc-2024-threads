// Copyright 2024 Kosarev Egor
#include "omp/kosarev_e_jarvis_hull/include/ops_omp.hpp"

#include <algorithm>
#include <stack>
#include <vector>
#include <random>

int orientation(Point p, Point q, Point r) {
  int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
  if (val == 0) return 0;
  return (val > 0) ? 1 : 2;
}

bool compare(Point p1, Point p2, Point p0) {
  int o = orientation(p0, p1, p2);
  if (o == 0)
    return ((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y)) <
           ((p2.x - p0.x) * (p2.x - p0.x) + (p2.y - p0.y) * (p2.y - p0.y));
  return (o == 2);
}

Point findFirstPoint(const std::vector<Point>& points) {
  Point first = points[0];
  for (const Point& p : points) {
    if (p.y < first.y || (p.y == first.y && p.x < first.x)) {
      first = p;
    }
  }
  return first;
}

std::stack<Point> convexHull(std::vector<Point>& points) {
  std::stack<Point> hull;
  if (points.size() < 3) {
    return hull;
  }

  Point p0 = findFirstPoint(points);

  std::sort(points.begin(), points.end(), [p0](const Point& p1, const Point& p2) { return compare(p1, p2, p0); });

  hull.push(points[0]);
  hull.push(points[1]);

  for (size_t i = 2; i < points.size(); i++) {
    while (hull.size() > 1) {
      Point p2 = hull.top();
      hull.pop();
      Point p1 = hull.top();
      hull.pop();
      if (orientation(p1, p2, points[i]) == 2) {
        hull.push(p1);
        hull.push(p2);
        break;
      } else {
        hull.push(p1);
      }
    }
    hull.push(points[i]);
  }

  return hull;
}

Point findFirstPoint_omp(const std::vector<Point>& points) {
  Point first = points[0];
#pragma omp parallel for
  for (size_t i = 1; i < points.size(); i++) {
    {
      if (points[i].y < first.y || (points[i].y == first.y && points[i].x < first.x)) {
        first = points[i];
      }
    }
  }
  return first;
}

std::vector<Point> convexHull_omp(std::vector<Point>& points) {
  if (points.size() < 3) {
    return points;
  }

  Point p0 = findFirstPoint_omp(points);

  std::sort(points.begin(), points.end(), [p0](const Point& p1, const Point& p2) { return compare(p1, p2, p0); });

  std::vector<Point> hull(points.size());
  hull[0] = points[0];
  hull[1] = points[1];

  size_t hull_size = 2;

#pragma omp parallel for
  for (size_t i = 2; i < points.size(); i++) {
    size_t k = hull_size;
    while (k > 1 && orientation(hull[k - 2], hull[k - 1], points[i]) != 2) {
      --k;
    }
    hull_size = k;
    hull[hull_size++] = points[i];
  }

  hull.resize(hull_size);
  return hull;
}

bool TestTaskSequentialKosarevJarvisHull::pre_processing() {
  internal_order_test();
  // Init value for input and output
  points = std::vector<Point>(taskData->inputs_count[0]);

  auto* tmp_ptr_A = reinterpret_cast<Point*>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    points[i] = tmp_ptr_A[i];
  }
  return true;
}

bool TestTaskSequentialKosarevJarvisHull::validation() {
  internal_order_test();
  // Check count elements of output
  return true;
}

bool TestTaskSequentialKosarevJarvisHull::run() {
  internal_order_test();
  pointsHull = convexHull(points);
  return true;
}
bool TestTaskSequentialKosarevJarvisHull::post_processing() {
  internal_order_test();

  std::vector<Point> tempVec;
  while (!pointsHull.empty()) {
    tempVec.push_back(pointsHull.top());
    pointsHull.pop();
  }

  std::copy(tempVec.begin(), tempVec.end(), reinterpret_cast<Point*>(taskData->outputs[0]));
  return true;
}

bool TestOMPTaskParallelKosarevJarvisHull::pre_processing() {
  internal_order_test();
  // Init value for input and output
  points = std::vector<Point>(taskData->inputs_count[0]);

  auto* tmp_ptr_A = reinterpret_cast<Point*>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    points[i] = tmp_ptr_A[i];
  }
  return true;
}

bool TestOMPTaskParallelKosarevJarvisHull::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] >= taskData->outputs_count[0];
}

bool TestOMPTaskParallelKosarevJarvisHull::run() {
  internal_order_test();
  pointsHull = convexHull(points);
  return true;
}
bool TestOMPTaskParallelKosarevJarvisHull::post_processing() {
  internal_order_test();

  std::vector<Point> tempVec;
  while (!pointsHull.empty()) {
    tempVec.push_back(pointsHull.top());
    pointsHull.pop();
  }

  std::copy(tempVec.begin(), tempVec.end(), reinterpret_cast<Point*>(taskData->outputs[0]));
  return true;
}