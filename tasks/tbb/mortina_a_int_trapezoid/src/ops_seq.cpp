// Copyright 2024 Mortina Nastya
#include <tbb/tbb.h>

#include <cmath>
#include <functional>
#include <iostream>
#include <thread>

#include "tbb/mortina_a_int_trapezoid/include/ops_tbb.hpp"

using namespace std::chrono_literals;

double trapezoidal_integral(double a1, double b1, double a2, double b2, int n1, int n2,
                            std::function<double(double, double)> f) {
  double h1 = (b1 - a1) / n1;
  double h2 = (b2 - a2) / n2;
  double integral = 0.0;

  for (int i = 0; i < n1; ++i) {
    for (int j = 0; j < n2; ++j) {
      double x0 = a1 + i * h1;
      double x1 = a1 + (i + 1) * h1;
      double y0 = a2 + j * h2;
      double y1 = a2 + (j + 1) * h2;

      double f00 = f(x0, y0);
      double f01 = f(x0, y1);
      double f10 = f(x1, y0);
      double f11 = f(x1, y1);

      integral += ((f00 + f01 + f10 + f11) / 4.0) * (x1 - x0) * (y1 - y0);
    }
  }

  return integral;
}

double trapezoidal_integral_tbb(double a1, double b1, double a2, double b2, int n1, int n2,
                                std::function<double(double, double)> f) {

  double h1 = (b1 - a1) / n1;
  double h2 = (b2 - a2) / n2;
  double result = 0;

  result = tbb::parallel_reduce(
      tbb::blocked_range2d<int>(0, n1, 0, n2), 0.0,
      [&](const tbb::blocked_range2d<int>& r, double res) {
        for (int i = r.rows().begin(); i != r.rows().end(); ++i) {
          for (int j = r.cols().begin(); j != r.cols().end(); ++j) {
            double x0 = a1 + i * h1;
            double x1 = a1 + (i + 1) * h1;

            double y0 = a2 + j * h2;
            double y1 = a2 + (j + 1) * h2;

            double f00 = f(x0, y0);
            double f01 = f(x0, y1);
            double f10 = f(x1, y0);
            double f11 = f(x1, y1);

            res += ((f00 + f01 + f10 + f11) / 4.0) * (x1 - x0) * (y1 - y0);
          }
        }
    return res;
      },
      std::plus<double>());

  return result;
}

// double trapezoidal_integral_tbb(double a1, double b1, double a2, double b2, int n1, int n2,
//                             std::function<double(double, double)> f) {
//   double h1 = (b1 - a1) / n1;
//   double h2 = (b2 - a2) / n2;
//   double integral = 0.0;

//   tbb::parallel_for(0, n1, [&](int i) {
//     double x0 = a1 + i * h1;
//     double x1 = a1 + (i + 1) * h1;

//     for (int j = 0; j < n2; ++j) {
//       double y0 = a2 + j * h2;
//       double y1 = a2 + (j + 1) * h2;

//       double f00 = f(x0, y0);
//       double f01 = f(x0, y1);
//       double f10 = f(x1, y0);
//       double f11 = f(x1, y1);

//       integral += ((f00 + f01 + f10 + f11) / 4.0) * (x1 - x0) * (y1 - y0);
//     }
//   });

//   return integral;
// }

bool TestTBBTaskSequentialMortinaIntegralTrapezoid::pre_processing() {
  internal_order_test();
  // Init value for input and output
  a1 = reinterpret_cast<double*>(taskData->inputs[0])[0];
  b1 = reinterpret_cast<double*>(taskData->inputs[0])[1];
  a2 = reinterpret_cast<double*>(taskData->inputs[0])[2];
  b2 = reinterpret_cast<double*>(taskData->inputs[0])[3];
  n1 = reinterpret_cast<int*>(taskData->inputs[1])[0];
  n2 = reinterpret_cast<int*>(taskData->inputs[1])[1];

  return true;
}

bool TestTBBTaskSequentialMortinaIntegralTrapezoid::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] == 4 && taskData->inputs_count[1] == 2 && taskData->outputs_count[0] == 1;
}

bool TestTBBTaskSequentialMortinaIntegralTrapezoid::run() {
  internal_order_test();
  res = trapezoidal_integral(a1, b1, a2, b2, n1, n2, fun);
  std::this_thread::sleep_for(30ms);
  return true;
}

bool TestTBBTaskSequentialMortinaIntegralTrapezoid::post_processing() {
  internal_order_test();
  reinterpret_cast<double*>(taskData->outputs[0])[0] = res;
  return true;
}

bool TestTBBTaskParallelMortinaIntegralTrapezoid::pre_processing() {
  internal_order_test();
  // Init vectors
  a1 = reinterpret_cast<double*>(taskData->inputs[0])[0];
  b1 = reinterpret_cast<double*>(taskData->inputs[0])[1];
  a2 = reinterpret_cast<double*>(taskData->inputs[0])[2];
  b2 = reinterpret_cast<double*>(taskData->inputs[0])[3];
  n1 = reinterpret_cast<int*>(taskData->inputs[1])[0];
  n2 = reinterpret_cast<int*>(taskData->inputs[1])[1];
  return true;
}

bool TestTBBTaskParallelMortinaIntegralTrapezoid::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->inputs_count[0] == 4 && taskData->inputs_count[1] == 2 && taskData->outputs_count[0] == 1;
}

bool TestTBBTaskParallelMortinaIntegralTrapezoid::run() {
  internal_order_test();
  res = trapezoidal_integral_tbb(a1, b1, a2, b2, n1, n2, fun);
  std::this_thread::sleep_for(30ms);
  return true;
}

bool TestTBBTaskParallelMortinaIntegralTrapezoid::post_processing() {
  internal_order_test();
  reinterpret_cast<double*>(taskData->outputs[0])[0] = res;
  return true;
}