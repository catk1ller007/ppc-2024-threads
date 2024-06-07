// Copyright 2024 Borovkov Sergey
#include "stl/borovkov_s_can_stl/include/ops_stl.hpp"

using namespace std::chrono_literals;

namespace BorovkovStl {
enum Order : size_t { MATR_ONE = 0, MATR_TWO = 1, SIZE = 2, BLOCK = 3, MATR_RES = 0 };

bool isEqual(double valueOne, double valueTwo, double eps) { return std::fabs(valueOne - valueTwo) <= eps; }

bool validateMatrix(size_t sizeOne, size_t sizeTwo) { return sizeOne == sizeTwo && sizeOne != 0; }

std::vector<double> CannonMatrixMultSeq(const std::vector<double>& matrOne, const std::vector<double>& matrTwo,
                                        int size, int block) {

  if (!validateMatrix(matrOne.size(), matrTwo.size())) throw std::invalid_argument{"invalid matrixs"};

  if (block > size) throw std::invalid_argument{"Wrong size block"};

  int jbMin = 0;
  int kbMin = 0;
  std::vector<double> matrRes(size * size, 0.0);

  for (int jb = 0; jb < size; jb += block) {
    for (int kb = 0; kb < size; kb += block) {
      jbMin = std::min(jb + block, size);
      kbMin = std::min(kb + block, size);

      for (int i = 0; i < size; ++i)
        for (int k = kb; k < kbMin; ++k)
          for (int j = jb; j < jbMin; ++j) matrRes[i * size + j] += matrOne[i * size + k] * matrTwo[k * size + j];
    }
  }

  return matrRes;
}

void blockMultiply(const std::vector<double>& matrOne, const std::vector<double>& matrTwo, std::vector<double>& matrRes, 
                   int size, int block, int startRow, int endRow) {
    for (int jb = 0; jb < size; jb += block) {
        for (int kb = 0; kb < size; kb += block) {
            int jbMin = std::min(jb + block, size);
            int kbMin = std::min(kb + block, size);

            for (int i = startRow; i < endRow; ++i) {
                for (int k = kb; k < kbMin; ++k) {
                    for (int j = jb; j < jbMin; ++j) {
                        matrRes[i * size + j] += matrOne[i * size + k] * matrTwo[k * size + j];
                    }
                }
            }
        }
    }
}

std::vector<double> CannonMatrixMultStl(const std::vector<double>& matrOne, const std::vector<double>& matrTwo, 
                                        int size, int block) {
    if (!validateMatrix(matrOne.size(), matrTwo.size())) 
        throw std::invalid_argument{"invalid matrix sizes"};

    if (block > size) 
        throw std::invalid_argument{"Wrong block size"};

    int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;

    std::vector<double> matrRes(size * size, 0.0);
    std::vector<std::thread> threads;
    int rowsPerThread = size / numThreads;

    for (int t = 0; t < numThreads; ++t) {
        int startRow = t * rowsPerThread;
        int endRow = (t == numThreads - 1) ? size : startRow + rowsPerThread;
        threads.emplace_back(blockMultiply, std::ref(matrOne), std::ref(matrTwo), std::ref(matrRes), size, block, startRow, endRow);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return matrRes;
}

std::vector<double> getRandomSquareMatrix(size_t size, double minVal, double maxVal) {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dist(minVal, maxVal);

  std::vector<double> matrix(size * size);
  for (auto& elem : matrix) elem = dist(gen);

  return matrix;
}

bool BorovkovCanMultMatrStl::pre_processing() {
  internal_order_test();
  // Init value for input and output.

  mMatrOne.resize(taskData->inputs_count[MATR_ONE]);
  mMatrTwo.resize(taskData->inputs_count[MATR_TWO]);

  mSize = *reinterpret_cast<size_t*>(taskData->inputs[SIZE]);
  mBlock = *reinterpret_cast<size_t*>(taskData->inputs[BLOCK]);

  size_t countElem = mSize * mSize;

  auto* ptrOne = reinterpret_cast<double*>(taskData->inputs[MATR_ONE]);
  auto* ptrTwo = reinterpret_cast<double*>(taskData->inputs[MATR_TWO]);

  for (size_t i = 0; i < countElem; ++i) {
    mMatrOne[i] = ptrOne[i];
    mMatrTwo[i] = ptrTwo[i];
  }

  return true;
}

bool BorovkovCanMultMatrStl::validation() {
  internal_order_test();
  return taskData->inputs_count[MATR_ONE] == taskData->inputs_count[MATR_TWO] &&
         taskData->inputs_count[MATR_ONE] == taskData->outputs_count[MATR_RES];
}

bool BorovkovCanMultMatrStl::run() {
  internal_order_test();

  try {
    mMatrRes = CannonMatrixMultStl(mMatrOne, mMatrTwo, mSize, mBlock);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return false;
  }

  return true;
}

bool BorovkovCanMultMatrStl::post_processing() {
  internal_order_test();
  std::copy(mMatrRes.begin(), mMatrRes.end(), reinterpret_cast<double*>(taskData->outputs[MATR_RES]));
  return true;
}
}  // namespace BorovkovStl