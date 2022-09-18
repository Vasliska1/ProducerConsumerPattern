#include "producer_consumer.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <cstring>

#define gettid() syscall(SYS_gettid)

using namespace std;

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_prod = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_cons = PTHREAD_COND_INITIALIZER;
thread_local unique_ptr<size_t> unic_tid;
int sleep_max = 0;

static bool debug_flag = false;
volatile atomic_bool finished = false;
volatile atomic_bool already_read = false;

static int consumed_counter;

struct inter_cl {
  int id;
  vector<pthread_t> consumers;
  int count;

  inter_cl(int id, vector<pthread_t> consumers, int count)
      : id(id), consumers(consumers), count(count) {}
};

struct prod_cons_cl {
  int *sh;
  int sleep;
  vector<int> numbers_vector;
  prod_cons_cl(int *sh, int sleep, vector<int> numbers_vector)
      : sh(sh), sleep(sleep), numbers_vector(numbers_vector) {}
};

int get_tid() {
  static atomic_size_t count = 0;
  unic_tid = make_unique<size_t>(count++);
  return *unic_tid;
}

void *producer_routine(void *arg) {
  auto *val = static_cast<prod_cons_cl *>(arg);

  finished = false;

  for (size_t i = 0; i < (val->numbers_vector).size(); ++i) {
    pthread_mutex_lock(&global_mutex);
    *(val->sh) = (val->numbers_vector)[i];
    already_read = false;
    pthread_cond_signal(&cond_cons);

    while (!already_read) {
      pthread_cond_wait(&cond_prod, &global_mutex);
    }

    pthread_mutex_unlock(&global_mutex);
  };

  finished = true;
  pthread_mutex_lock(&global_mutex);
  pthread_cond_broadcast(&cond_cons);
  pthread_mutex_unlock(&global_mutex);

  pthread_exit(0);
}

void *consumer_routine(void *arg) {
  auto *val = static_cast<prod_cons_cl *>(arg);

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

  int *p_sum = new int(0);

  while (!finished) {
    pthread_mutex_lock(&global_mutex);

    while (!finished && already_read) {
      pthread_cond_wait(&cond_cons, &global_mutex);
      if (finished) break;
    }

    if (!already_read) {
      already_read = true;

      *p_sum += *val->sh;
      if (debug_flag) cout << get_tid() << " , " << *(p_sum) << endl;

      consumed_counter++;
    }
    pthread_cond_signal(&cond_prod);
    pthread_mutex_unlock(&global_mutex);
    if (val->sleep != 0) {
      this_thread::sleep_for(chrono::milliseconds(rand() % val->sleep));
    }
  }
  return (void *)p_sum;
}

void *consumer_interruptor_routine(void *arg) {
  auto *val = static_cast<inter_cl *>(arg);
  while (!finished) {
    int random_index = rand() % (val->count);
    pthread_cancel(val->consumers[random_index]);
  }

  // interrupt random consumer while producer is running

  return nullptr;
}

int run_threads(int number_threads, int time_sleep, bool debug,
                vector<int> numbers_vector) {
  debug_flag = debug;
  int shared = 0;

  pthread_t producer;
  pthread_t interrupter;
  vector<pthread_t> consumer(number_threads);

  struct prod_cons_cl *producer_consumer_struct =
      new prod_cons_cl(&shared, time_sleep, numbers_vector);

  pthread_create(&producer, nullptr, &producer_routine,
                 (void *)producer_consumer_struct);

  for (int i = 0; i < number_threads; ++i) {
    pthread_create(&consumer[i], nullptr, &consumer_routine,
                   (void *)producer_consumer_struct);
  }
  struct inter_cl *inter_struct = new inter_cl(2, consumer, number_threads);

  pthread_create(&interrupter, nullptr, consumer_interruptor_routine,
                 (void *)inter_struct);

  pthread_join(producer, nullptr);
  pthread_join(interrupter, nullptr);

  int i = 0;
  int end_sum = 0;

  while (i < number_threads) {
    int *part_sum;
    pthread_join(consumer[i], (void **)&part_sum);
    end_sum += *part_sum;
    i++;
    delete part_sum;
  }

  pthread_mutex_destroy(&global_mutex);
  pthread_cond_destroy(&cond_cons);
  pthread_cond_destroy(&cond_prod);
  consumed_counter = 0;

  delete (producer_consumer_struct);
  delete (inter_struct);
  return end_sum;
}