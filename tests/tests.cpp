#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <producer_consumer.h>
#include <vector>

using namespace std;

TEST_CASE("just_example") { CHECK(4 == 4); }

TEST_CASE("testing 15 threads 1 sleep") {
  vector<int> testValues;
  testValues.push_back(1);
  testValues.push_back(2);
  testValues.push_back(3);

  CHECK(run_threads(15, 1, 0, testValues) == 6);
}

TEST_CASE("testing 100 threads and 10 sleep") {
  vector<int> testValues;
  testValues.push_back(0);

  CHECK(run_threads(100, 10, 0, testValues) == 0);
}

TEST_CASE("testing 200 and 5 sleep ") {
  vector<int> testValues;
  testValues.push_back(0);
  testValues.push_back(100);
  testValues.push_back(20);
  testValues.push_back(5);

  CHECK(run_threads(200, 5, 0, testValues) == 125);
}

TEST_CASE("testing 1000 and 0 sleep ") {
  vector<int> testValues;
  testValues.push_back(100);
  testValues.push_back(100);
  testValues.push_back(100);
  testValues.push_back(5500);
  testValues.push_back(1);
  testValues.push_back(3);

  CHECK(run_threads(1000, 0, 0, testValues) == 5804);
}