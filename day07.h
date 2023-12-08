//
// Created by matze on 06/12/2023.
//

#ifndef AOCC_DAY07_H
#define AOCC_DAY07_H

#include "helpers.h"
#include "two_part_result.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define DAY07_FILE "/tmp/day07"

// TODO: remove this define.solve both parts simultaneously
#define DAY07_PART2

#define HAND_SIZE 5

#define CMP(a, b) (((a) > (b)) - ((b) > (a)))

struct hand {
  int largest_match;
  int second_largest_match;
  char unsorted_hand[HAND_SIZE + 1];
  long bid;
};

char card_strength(char card) {
  switch (card) {
  case 'A':
    return 14;
  case 'K':
    return 13;
  case 'Q':
    return 12;
#ifdef DAY07_PART2
  case 'J':
    return 1;
#else
  case 'J':
    return 11;
#endif
  case 'T':
    return 10;
  default:
    return card - '0';
  }
}

int compare_char(const char *a, const char *b) { return (*a > *b) - (*b > *a); }

int compare_card_strength(const char *c1, const char *c2) {
  const char c1_strength = card_strength(*c1);
  const char c2_strength = card_strength(*c2);
  return compare_char(&c1_strength, &c2_strength);
}

// void print_hand(const struct hand *h) {
//  printf("hand:\n\t%d\n\t%d\n\t%s\n\t%ld\n", h->largest_match,
//         h->second_largest_match, h->unsorted_hand, h->bid);
//}

int compare_hand(const struct hand *h1, const struct hand *h2) {
  const int largest_match_diff = CMP(h1->largest_match, h2->largest_match);
  if (largest_match_diff != 0)
    return largest_match_diff;
  const int second_largest_match_diff =
      CMP(h1->second_largest_match, h2->second_largest_match);
  if (second_largest_match_diff != 0)
    return second_largest_match_diff;
  for (int i = 0; i < 5; i++) {
    const int curr_card_compare =
        compare_card_strength(h1->unsorted_hand + i, h2->unsorted_hand + i);
    if (curr_card_compare != 0)
      return curr_card_compare;
  }
  return 0;
}

void parse_hand(char *line, struct hand *out) {
  char sorted_hand[HAND_SIZE + 1] = {};

  // parse
  memcpy(sorted_hand, line, HAND_SIZE);
  memcpy(out->unsorted_hand, line, HAND_SIZE);
  out->unsorted_hand[HAND_SIZE] = 0;
  out->bid = strtol(line + 5, NULL, 10);

  // check matches
  qsort(sorted_hand, HAND_SIZE, sizeof(char),
        (__compar_fn_t)compare_card_strength);
  char *end = sorted_hand + 5;
  int run_length = 1;
  int match1 = 0;
  int match2 = 0;
  char prior_val = 0;
  char *curr_pos = sorted_hand;
  int jokers = 0;
#ifdef DAY07_PART2
  while (*curr_pos == 'J') {
    jokers += 1;
    curr_pos++;
  }
#endif
  for (; curr_pos != end; ++curr_pos) {
    if (*curr_pos == prior_val) {
      run_length += 1;
    } else {
      if (run_length > 1) {
        if (match1 == 0) {
          match1 = run_length;
        } else {
          match2 = run_length;
        }
        run_length = 1;
      }
    }
    prior_val = *curr_pos;
  }
  if (run_length > 1)
    match2 = run_length;
  out->largest_match = max(match1, match2);
  out->second_largest_match = min(match1, match2);
  if (jokers > 0) {
    if (out->largest_match == 0) {
      out->largest_match = min(1 + jokers, 5);
    } else {
      out->largest_match += jokers;
    }
  }
}

struct two_part_result *day07(char *buf, __attribute__((unused)) long buf_len) {
  struct two_part_result *day_res = allocate_two_part_result();

  struct hand curr_hand;
  struct hand *hands = NULL;
  char **curr_pos = &buf;
  while (true) {
    char *const line = strsep(curr_pos, "\n");
    if (*line == 0)
      break;
    parse_hand(line, &curr_hand);
    cvector_push_back(hands, curr_hand);
  }

  qsort(hands, cvector_size(hands), sizeof(*hands),
        (__compar_fn_t)compare_hand);

  long long winnings = 0;

  int i = 1;
  for (struct hand *it = cvector_begin(hands); it != cvector_end(hands); ++it) {
    winnings += i * (it->bid);
    i++;
  }
#ifdef DAY07_PART2
  day_res->part2_result = winnings;
#else
  day_res->part1_result = winnings;
#endif
  return day_res;
}

void solve_day07() {
  struct two_part_result *day_res;
  char *input_buffer;
  const long filesize = read_file_to_memory(DAY07_FILE, &input_buffer, false);
  day_res = day07(input_buffer, filesize);
  print_day_result("day07", day_res);
  free_two_part_result(day_res);
  free(input_buffer);
}

#endif // AOCC_DAY07_H
