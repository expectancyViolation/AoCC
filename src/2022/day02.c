#include "day02.h"

enum RPS { RPS_rock = 0, RPS_paper = 1, RPS_scissors = 2 };

enum OUTCOME { OUTCOME_loss = 0, OUTCOME_draw = 3, OUTCOME_win = 6 };

enum RPS p1_parse_opponent(char c) {
  switch (c) {
  case 'A':
    return RPS_rock;
  case 'B':
    return RPS_paper;
  case 'C':
    return RPS_scissors;
  }
  assert(false);
}

enum RPS p1_parse_me(char c) {
  switch (c) {
  case 'X':
    return RPS_rock;
  case 'Y':
    return RPS_paper;
  case 'Z':
    return RPS_scissors;
  }
  assert(false);
}

enum OUTCOME p2_parse_outcome(char c) {
  switch (c) {
  case 'X':
    return OUTCOME_loss;
  case 'Y':
    return OUTCOME_draw;
  case 'Z':
    return OUTCOME_win;
  }
  assert(false);
}

int RPS_compare(enum RPS rps1, enum RPS rps2) {
  return ((4 + (int)rps1 - (int)rps2) % 3) - 1;
}

enum OUTCOME score_outcome(enum RPS me, enum RPS opponent) {
  switch (RPS_compare(me, opponent)) {
  case -1:
    return OUTCOME_loss;
  case 0:
    return OUTCOME_draw;
  case 1:
    return OUTCOME_win;
  default:
    assert(false);
  }
}

enum RPS solve_for_outcome(enum RPS opponent_move, enum OUTCOME outcome) {
  const int signed_diff = (2 + (((int)outcome) / 3) + (int)opponent_move);
  return (enum RPS)((6 + signed_diff) % 3);
}

int score_round(enum RPS me, enum RPS opponent) {
  const int my_move_score = 1 + (int)me;
  return ((int)score_outcome(me, opponent)) + my_move_score;
};

LLTuple year22_day02(char *buf, long buf_len) {
  LLTuple result = {AOC_LLONG_NO_ANSWER, AOC_LLONG_NO_ANSWER};
  char **buf_pos = &buf;
  long long p1_score = 0;
  long long p2_score = 0;
  while (*buf_pos != NULL) {
    char *line = strsep(buf_pos, "\n");
    if ((line == NULL) || (*line) == 0)
      break;
    enum RPS opponent_move = p1_parse_opponent(line[0]);
    enum RPS p1_my_move = p1_parse_me(line[2]);
    p1_score += score_round(p1_my_move, opponent_move);

    enum OUTCOME p2_outcome = p2_parse_outcome(line[2]);
    p2_score += score_round(solve_for_outcome(opponent_move, p2_outcome),
                            opponent_move);
  }
  result.left = p1_score;
  result.right = p2_score;
  return result;
}
AocDayRes solve_year22_day02(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  LLTuple res = year22_day02(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
