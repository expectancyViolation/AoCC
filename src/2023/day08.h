#ifndef AOCC_DAY08_H
#define AOCC_DAY08_H

#include "../util/aoc.h"
#include "../util/helpers.h"
#include "../util/two_part_result.h"

#define _GNU_SOURCE
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_LEN 3

struct node {
  struct node *left;
  struct node *right;
};

void pos_to_key(size_t val, char *key) {
  for (int i = 0; i < KEY_LEN; i++) {
    *(key++) = (val % 26) + 'A';
    val /= 26;
  }
}

size_t key_to_pos(char *key) {
  return (key[0] - 'A') + 26 * (key[1] - 'A' + 26 * (key[2] - 'A'));
}

struct node *parse_node(struct node *nodes, char *line) {
  const size_t node_val = key_to_pos(line);
  const size_t left_val = key_to_pos(line + 7);
  const size_t right_val = key_to_pos(line + 12);
  struct node *curr_node = nodes + node_val;
  curr_node->left = nodes + left_val;
  curr_node->right = nodes + right_val;
  return curr_node;
}

void parse_nodes(char *buf, struct node *nodes, size_t **start_positions) {
  char *line;
  struct node *curr_node = NULL;

  while (true) {
    line = strsep(&buf, "\n");
    if (buf == NULL)
      break;
    bool ends_with_a = line[2] == 'A';
    curr_node = parse_node(nodes, line);
    if (ends_with_a)
      cvector_push_back(*start_positions, curr_node - nodes);
  }
}

struct two_part_result *day08(char *buf, __attribute__((unused)) long buf_len) {
  struct two_part_result *day_res = allocate_two_part_result();

  struct node nodes[26 * 26 * 26]; // 26**KEY_LEN   no constexpr in C :(
  struct node *curr_node = NULL;
  size_t *start_positions = NULL;

  // parse
  char *const rl_seq = strsep(&buf, "\n");
  const size_t rl_period = buf - rl_seq - 1;
  strsep(&buf, "\n"); // discard empty line
  parse_nodes(buf, nodes, &start_positions);

  // solve
  long long res = 1;
  struct node *target = nodes + key_to_pos("ZZZ");
  const size_t aaa_pos = key_to_pos("AAA");
  for (size_t i = 0; i < cvector_size(start_positions); i++) {
    curr_node = nodes + start_positions[i];
    long long j;
    char b[4] = {};
    for (j = 0; curr_node != target; j++) {
      // input is easier than general case:
      //  first goal state (at step k) loops back onto second element
      //  => period (of size k) is entered at second element
      //  => sequence is only at goal at step k*n
      // this also implies that k is solution to part 1:
      // since we only enter one **Z state=> it has to be ZZZ
      pos_to_key((curr_node - nodes), b);
      if (b[2] == 'Z') {
        break;
      }
      switch (rl_seq[j % rl_period]) {
      case 'L':
        curr_node = curr_node->left;
        break;
      case 'R':
        curr_node = curr_node->right;
        break;
      default:
        assert(false);
      }
    }
    if (start_positions[i] == aaa_pos) {
      day_res->part1_result = j;
    }
    // calculate lcm of periods
    res = (res * j) / gcd(res, j);
  }
  day_res->part2_result = res;
  return day_res;
}

void solve_day08() {
  const int year = 2023;
  const int day = 8;
  struct two_part_result *day_res;
  char *input_buffer;
  // const long filesize = read_file_to_memory(DAY08_FILE, &input_buffer,false);
  const long filesize = get_day_input_cached(year, day, &input_buffer);
  day_res = day08(input_buffer, filesize);
  print_day_result(day, day_res);

  // part 1
  // submit_answer(year, day, day_part_part1, day_res);

  // part 2
  // submit_answer(year, day, day_part_part2, day_res);

  free_two_part_result(day_res);
  free(input_buffer);
}

#endif // AOCC_DAY08_H
