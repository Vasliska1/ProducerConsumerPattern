#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

constexpr size_t const DEFAULT_THREADS = 3;
using ll = long long;
// the declaration of run threads can be changed as you like
int run_threads(int number_threads, int sleep, bool debug_flag,
                std::vector<int> numbers_vector);
