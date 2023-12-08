//
// Created by matze on 06/12/2023.
//

#ifndef AOCC_DAY08_H
#define AOCC_DAY08_H

#include "helpers.h"
#include "two_part_result.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define DAY08_FILE "/tmp/day08"

#define KEY_LEN 3

struct node {
    struct node *left;
    struct node *right;
};


void int_to_key(int val, char *key) {
    for (int i = 0; i < 3; i++) {
        *(key++) = (val % 26) + 'A';
        val /= 26;
    }
}

void print_node(const struct node *nodes, const struct node *node) {
    char b[4] = {0};
    ptrdiff_t d = node - nodes;
    int_to_key(d, b);
    printf("node: %s\n", b);
    d = (node->left) - nodes;
    int_to_key(d, b);
    printf("left: %s\n", b);

    d = (node->right) - nodes;
    int_to_key(d, b);
    printf("right: %s\n", b);
}

int key_to_int(char *key) {
    return (key[0] - 'A') + 26 * (key[1] - 'A' + 26 * (key[2] - 'A'));
}


struct node *parse_node(struct node *nodes, char *line) {
    const int node_val = key_to_int(line);
    const int left_val = key_to_int(line + 7);
    const int right_val = key_to_int(line + 12);
    struct node *curr_node = nodes + node_val;
    curr_node->left = nodes + left_val;
    curr_node->right = nodes + right_val;
    return curr_node;
}

struct two_part_result *day08(char *buf, __attribute__((unused)) long buf_len) {
    struct two_part_result *day_res = allocate_two_part_result();
    struct node nodes[26 * 26 * 26];
    char* line;
    struct node *curr_node = NULL;
    size_t *start_nodes = NULL;
    struct node *target = nodes + key_to_int("ZZZ");
    char *const rl_seq = strsep(&buf, "\n");
    const int rl_max = buf - rl_seq - 1;
    strsep(&buf, "\n");
    while (true) {
        line = strsep(&buf, "\n");
        if (buf == NULL)
            break;
        bool ends_with_a = line[2] == 'A';
        curr_node = parse_node(nodes, line);
        if (ends_with_a)
            cvector_push_back(start_nodes, curr_node - nodes);
    }
    long long res = 1;
    for (int i = 0; i < cvector_size(start_nodes); i++) {
        curr_node = nodes + start_nodes[i];
        long j;
        int prev_j = 0;
        char b[4] = {};
        for (j = 0; curr_node != target; j++) {
            int d = curr_node - nodes;
            int_to_key(d, b);
            // input is easier than general case:
            //  first goal state (at step k) loops back onto second element=>period (of size k) is entered at second element
            //  sequence is only at goal at step k*n
            // this also implies that k is solution to part 1 (since we only enter one **Z state=> it has to be ZZZ)
            if (b[2] == 'Z') {
                break;
            }
            switch (rl_seq[j % rl_max]) {
                case 'L':
                    curr_node = curr_node->left;
                    break;
                case 'R':
                    curr_node = curr_node->right;
                    break;
                default:
                    assert(false);
            };
        }
        if(start_nodes[i]== key_to_int("AAA")){
            day_res->part1_result=j;
        }
        res = (res * j) / gcd(res, j);
    }
    day_res->part2_result = res;
    return day_res;
}

void solve_day08() {
    struct two_part_result *day_res;
    char *input_buffer;
    const long filesize = read_file_to_memory(DAY08_FILE, &input_buffer, false);
    day_res = day08(input_buffer, filesize);
    print_day_result("day08", day_res);
    free_two_part_result(day_res);
    free(input_buffer);
}

#endif // AOCC_DAY08_H
