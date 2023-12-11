#include "day04.h"

#include "../util/fenwick.h"
#include "../util/parallelize.h"

#include <stdbool.h>

#define DAY04_MIN_LINE_LEN 10 // assume a line is at least 10 bytes long
#define MAX_WIN_NUMBER 100    // assume winning numbers are 2 digit
#define num_bitmask 0b1111

// #define DAY04_UNSAFE_PARALLEL

__attribute__((unused)) static void parse_winning(char *line,
                                                  char *winning_numbers) {
  while (line != NULL) {
    const long win_num = strtol(line, &line, 10);
    if (win_num == 0)
      break;

    winning_numbers[win_num] = 1;
  }
}

static void parse_winning_brittle(char *line, char *winning_numbers) {
  while (*line != 0) {
    line++;
    int val = ((*(line++)) & num_bitmask) * 10;
    val += (((*(line++)) & num_bitmask));
    winning_numbers[val] += 1;
  }
}

__attribute__((unused)) static int count_matches(char *ticket,
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

static int count_matches_brittle(char *ticket, const char *winning_numbers) {
  int curr_matches = 0;
  while (*ticket != 0) {
    ticket++;
    int val = ((*(ticket++)) & num_bitmask) * 10;
    val += (((*(ticket++)) & num_bitmask));
    curr_matches += winning_numbers[val];
  }
  return curr_matches;
}
LLTuple year23_day04(char *buf, long buf_len) {
  LLTuple result = {};
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
    result.left += curr_res;

    const long won_count = fenwick_get(range_tree, pos) + 1;
    fenwick_add_range(range_tree, tree_size, pos + 1, pos + 1 + curr_matches,
                      won_count);
    result.right += won_count;
    pos += 1;
  }
  free(range_tree);
  return result;
}
AocDayRes solve_year23_day04(const char *input_file) {
  char *input_buffer;
  // harder to parallelize b.c. of arbitrary Day05Range interactions!
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
#ifdef DAY04_UNSAFE_PARALLEL
  const LLTuple day_res =
      parallelize((day04), ll_tuple_add, input_buffer, filesize, 0);
#else
  const LLTuple res = year23_day04(input_buffer, filesize);
#endif
  AocDayRes day_res = {res};
  free(input_buffer);
  return day_res;
}
