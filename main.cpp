#include <iostream>
#include "producer_consumer.h"

using namespace std;

int main(int argc, char** argv) {
  bool debug_flag = false;
  if (argc < 3 || argc > 5) {
    std::cout << "not right count arg";
    return -1;
  }
  if (argc == 4) {
    if (std::string(argv[3]) == "-debug") debug_flag = true;
  }
  int count_threads = std::stoi(argv[1]);
  int sleep = std::stoi(argv[2]);

  string line;
  int number;
  vector<int> numbers_vector;
  getline(cin, line);
  stringstream iss(line);
  while (iss >> number) {
    numbers_vector.push_back(number);
  };

  int result_sum =
      run_threads(count_threads, sleep, debug_flag, numbers_vector);
  std::cout << result_sum << "\n";
  return 0;
}
