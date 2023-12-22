#include <assert.h>
#include <stdbool.h>
#include <time.h>

#include "timer.h"



#ifdef WIN32
#include <sysinfoapi.h>
int clock_gettime(int _, struct timespec *spec)      //C-file part
{  __int64 wintime; GetSystemTimeAsFileTime((FILETIME*)&wintime);
  wintime      -=116444736000000000i64;  //1jan1601 to 1jan1970
  spec->tv_sec  =wintime / 10000000i64;           //seconds
  spec->tv_nsec =wintime % 10000000i64 *100;      //nano-seconds
  return 0;
}
#endif

void start_my_perf_timer(struct my_perf_timer *timer) {
  assert(!timer->running);
  timer->running = true;
  clock_gettime(CLOCK_MONOTONIC_RAW, timer->begin);
}

void destroy_my_perf_timer(struct my_perf_timer *timer) {
  free(timer->begin);
  free(timer);
}

struct my_perf_timer *start_perf_measurement() {
  struct timespec *begin = malloc(sizeof *begin);
  struct my_perf_timer *timer = malloc(sizeof *timer);
  timer->begin = begin;
  timer->running = false;
  start_my_perf_timer(timer);
  return timer;
}

double stop_perf_measurement(struct my_perf_timer *timer) {
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  const double result =
      (double)(end.tv_nsec - timer->begin->tv_nsec) / 1000000000.0 +
      (double)(end.tv_sec - timer->begin->tv_sec);
  destroy_my_perf_timer(timer);
  return result;
}

void benchmark(void (*fun)()) {
  double took;
  struct my_perf_timer *timer = start_perf_measurement();
  fun();
  took = stop_perf_measurement(timer);
  printf("took: %f\n", took);
}
