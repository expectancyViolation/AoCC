//
// Created by matze on 04/12/2023.
//

#include <stdbool.h>
#include "two_part_result.h"
#include "helpers.h"
#include "parallelize.h"
#include "fenwick.h"

#ifndef AOCC_DAY04_H
#define AOCC_DAY04_H

struct two_part_result *day04(char *buf, long buf_len) {
    struct two_part_result *result = allocate_two_part_result();
    long range_tree[1000]={0};
    const int tree_size=1000;
    int pos=0;
    while (buf != NULL) {
        char *line = strsep(&buf, "\n");
        strsep(&line,":");
        char *ticket= strsep(&line,"|");
        int* winning_numbers = NULL;
        while(true){
            const int win_num = (int)strtol(line, &line, 10);
            if(win_num==0)
                break;

            cvector_push_back(winning_numbers, win_num);
        }
        int* it;
        for (it = cvector_begin(winning_numbers); it != cvector_end(winning_numbers); ++it) {
        }

        int curr_matches=0;
        while(true){
            const int curr_num = (int)strtol(ticket, &ticket, 10);
            if(curr_num==0)
                break;
            for (it = cvector_begin(winning_numbers); it != cvector_end(winning_numbers); ++it) {
                if(curr_num==*it){
                   curr_matches++;
                }
            }
        }
        const int curr_res=curr_matches?(1<<(curr_matches-1)):0;
        const long won_count=fenwick_get_element(range_tree,pos)+1;
        fenwick_add_range(range_tree,tree_size,pos,pos+curr_matches,won_count);
        printf("score is %d (won %ld)\n",curr_matches,won_count);
        result->part1_result+=curr_res;
        result->part2_result+=won_count;
        pos+=1;
    }

    return result;
}

void solve_day04() {
    struct two_part_result *day_res = allocate_two_part_result();
    char *input_buffer;
    const long filesize =
            read_file_to_memory("/tmp/day04", &input_buffer, true);
//    parallelize((void *(*)(char *, long))(day04),
//                (void (*)(void *, void *))(add_consume_partial_result), day_res,
//                input_buffer,filesize, 0);
    day_res=day04(input_buffer,filesize);
    print_day_result("day04", day_res);
    free_two_part_result(day_res);
    free(input_buffer);
}


#endif //AOCC_DAY04_H
