#include "day08.h"

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
LLTuple day08(char *buf, long buf_len) {
  LLTuple day_res = {};

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
      // this also implies that k is solution_type_num_solution to part 1:
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
      day_res.left = j;
    }
    // calculate lcm of periods
    res = (res * j) / gcd(res, j);
  }
  day_res.right = res;
  return day_res;
}
struct AocDayRes solve_day08(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer,false);
  const LLTuple res = day08(input_buffer, filesize);

  struct AocDayRes day_res={res};
  free(input_buffer);
  return day_res;
}
