#ifndef AOCC_DAY04_H
#define AOCC_DAY04_H

#include "../util/aoc.h"
#include "../util/fenwick.h"
#include "../util/helpers.h"
#include "../util/parallelize.h"
#include "../util/two_part_result.h"

#include <stdbool.h>

#define DAY04_MIN_LINE_LEN 10 // assume a line is at least 10 bytes long
#define MAX_WIN_NUMBER 100    // assume winning numbers are 2 digit
#define num_bitmask 0b1111

// #define DAY04_UNSAFE_PARALLEL

__attribute__((unused)) void parse_winning(char *line, char *winning_numbers) {
  while (line != NULL) {
    const long win_num = strtol(line, &line, 10);
    if (win_num == 0)
      break;

    winning_numbers[win_num] = 1;
  }
}

void parse_winning_brittle(char *line, char *winning_numbers) {
  while (*line != 0) {
    line++;
    int val = ((*(line++)) & num_bitmask) * 10;
    val += (((*(line++)) & num_bitmask));
    winning_numbers[val] += 1;
  }
}

__attribute__((unused)) int count_matches(char *ticket,
                                          const char *winning_numbers) {

  int curr_matches = 0;
  while (true) {
    const int curr_num = (int)strtol(ticket, &ticket, 10);
    if (curr_num == 0)
      break;
    curr_matches += winning_numbers[curr_num];
  }
  return curr_matches;
}

int count_matches_brittle(char *ticket, const char *winning_numbers) {
  int curr_matches = 0;
  while (*ticket != 0) {
    ticket++;
    int val = ((*(ticket++)) & num_bitmask) * 10;
    val += (((*(ticket++)) & num_bitmask));
    curr_matches += winning_numbers[val];
  }
  return curr_matches;
}

struct two_part_result *day04(char *buf, long buf_len) {
  struct two_part_result *result = allocate_two_part_result();
  const int tree_size =
      (int)(buf_len / DAY04_MIN_LINE_LEN); // this has to be larger than the
  // number of lines
  long *range_tree = malloc(sizeof(*range_tree) * tree_size);
  memset(range_tree, 0, sizeof(*range_tree) * tree_size);
  int pos = 0;
  char winning_numbers[MAX_WIN_NUMBER] = {0};
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    strsep(&line, ":");
    char *ticket = strsep(&line, "|");
    if (ticket == NULL) {
      // printf("skipping invalid ticket:%s(\n)", line);
      continue;
    }

    *(line - 2) = 0; // delimit ticket for brittle parsing
    parse_winning_brittle(line, winning_numbers);
    int curr_matches = count_matches_brittle(ticket, winning_numbers);

    // parse_winning(line, winning_numbers);
    // int curr_matches = count_matches(ticket, winning_numbers);

    memset(winning_numbers, 0, MAX_WIN_NUMBER * sizeof(*winning_numbers));

    const int curr_res = curr_matches ? (1 << (curr_matches - 1)) : 0;
    result->part1_result += curr_res;

    const long won_count = fenwick_get(range_tree, pos) + 1;
    fenwick_add_range(range_tree, tree_size, pos + 1, pos + 1 + curr_matches,
                      won_count);
    result->part2_result += won_count;
    pos += 1;
  }
  free(range_tree);
  return result;
}

void solve_day04() {
  const int year = 2023;
  const int day = 4;
  struct two_part_result *day_res; // = allocate_two_part_result();
  char *input_buffer;
  // harder to parallelize b.c. of arbitrary range interactions!
  const long filesize = get_day_input_cached(year, day, &input_buffer);
#ifdef DAY04_UNSAFE_PARALLEL
  parallelize((void *(*)(char *, long))(day04),
              (void (*)(void *, void *))(add_consume_partial_result), day_res,
              input_buffer, filesize, 0);
#else
  day_res = day04(input_buffer, filesize);
#endif
  print_day_result(day, day_res);

  // part 1
  // submit_answer(year, day, day_part_part1, day_res);

  // part 2
  // submit_answer(year, day, day_part_part2, day_res);
  free_two_part_result(day_res);
  free(input_buffer);
}

#endif // AOCC_DAY04_H
