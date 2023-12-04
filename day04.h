//
// Created by matze on 04/12/2023.
//

#include "fenwick.h"
#include "helpers.h"
#include "parallelize.h"
#include "two_part_result.h"
#include <stdbool.h>

#ifndef AOCC_DAY04_H
#define AOCC_DAY04_H

#define DAY04_MIN_LINE_LEN 10 // assume a line is at least 10 bytes long

struct two_part_result *day04(char *buf, long buf_len) {
    struct two_part_result *result = allocate_two_part_result();
    const int tree_size =
            (int) (buf_len /
                   DAY04_MIN_LINE_LEN); // this has to be larger than the number of lines
    long *range_tree = malloc(sizeof(*range_tree) * tree_size);
    int pos = 0;
    long *winning_numbers = NULL;
    while (buf != NULL) {
        char *line = strsep(&buf, "\n");
        strsep(&line, ":");
        char *ticket = strsep(&line, "|");
        if (ticket == NULL) {
            // printf("skipping invalid ticket:%s(\n)", line);
            continue;
        }
        while (line != NULL) {
            const long win_num = strtol(line, &line, 10);
            if (win_num == 0)
                break;

            cvector_push_back(winning_numbers, win_num);
        }

        ////    sorted search is not worth it for small line sizes
        //      sort:
        //        qsort(winning_numbers, cvector_size(winning_numbers),
        //        sizeof(long),
        //              (__compar_fn_t) compare_long);

        //      then find with:
        //            long const *m = bsearch(&curr_num, winning_numbers,
        //            cvector_size(winning_numbers), sizeof(long),
        //                                    (__compar_fn_t) compare_long);
        //            if (m)
        //                curr_matches++;

        int curr_matches = 0;
        long *it;
        while (true) {
            const int curr_num = (int) strtol(ticket, &ticket, 10);
            if (curr_num == 0)
                break;
            for (it = cvector_begin(winning_numbers);
                 it != cvector_end(winning_numbers); ++it) {
                if (curr_num == *it) {
                    curr_matches++;
                }
            }
        }
        cvector_clear(winning_numbers);

        const int curr_res = curr_matches ? (1 << (curr_matches - 1)) : 0;
        result->part1_result += curr_res;

        const long won_count = fenwick_get(range_tree, pos) + 1;
        fenwick_add_range(range_tree, tree_size, pos + 1, pos + 1 + curr_matches,
                          won_count);
        result->part2_result += won_count;
        pos += 1;
    }
    free(range_tree);
    cvector_free(winning_numbers);
    return result;
}

void solve_day04() {
    struct two_part_result *day_res;
    char *input_buffer;
    // harder to parallelize b.c. of arbitrary range interactions!
    const long filesize =
            read_file_to_memory("/tmp/day04_bigboy", &input_buffer, false);
    day_res = day04(input_buffer, filesize);
    print_day_result("day04", day_res);
    free_two_part_result(day_res);
    free(input_buffer);
}

#endif // AOCC_DAY04_H
