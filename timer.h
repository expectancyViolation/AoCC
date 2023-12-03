//
// Created by matze on 03/12/2023.
//

#ifndef AOCC_TIMER_H
#define AOCC_TIMER_H

#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <time.h>

struct my_perf_timer {
  struct timespec *begin;
  bool running;
};

struct my_perf_timer *start_perf_measurement();

double stop_perf_measurement(struct my_perf_timer *timer);

void benchmark(void (*fun)());

#endif // AOCC_TIMER_H
