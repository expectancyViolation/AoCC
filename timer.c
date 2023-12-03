//
// Created by matze on 03/12/2023.
//

#include <assert.h>
#include <stdbool.h>
#include <time.h>

#include "timer.h"

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
};
