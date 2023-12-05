#include "fenwick.h"
#include "helpers.h"
#include "parallelize.h"
#include "two_part_result.h"
#include "cvector.h"
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#ifndef AOCC_DAY05_H
#define AOCC_DAY05_H

#define DAY05_FILE "/tmp/day05"

void parse_seeds(char *line, long **vec) {

    strsep(&line, ":");
    char *endpos;
    while (true) {
        const long curr_num = strtol(line, &endpos, 10);
        if (endpos == line)
            break;
        line = endpos;
        printf("%ld\n", curr_num);
        cvector_push_back(*vec, curr_num);
    }
    printf("donso\n");
}

void print_arr(const long *arr, int arr_len) {
    for (int i = 0; i < arr_len; i++) {
        printf("%ld,", arr[i]);
    }
    printf("\n");
}

struct two_part_result *day05(char *buf, long buf_len) {
    struct two_part_result *day_res = allocate_two_part_result();
    long *source = NULL;
    char *line = strsep(&buf, "\n");
    parse_seeds(line, &source);
    const int seed_el_count = cvector_size(source);
    const size_t seed_size = seed_el_count * (sizeof(long));
    long *arr1 = malloc(seed_size);
    long *arr2 = malloc(seed_size);
    memcpy(arr1, source, seed_size);
    memcpy(arr2, source, seed_size);
    strsep(&buf, "\n"); // remove first newline
    while (true) {
        if (buf == NULL)
            break;
        char *map_title = strsep(&buf, "\n"); // remove first newline
        printf("title:%s\n", map_title);
        while (true) {
            char *endpos;
            long target_range = strtol(buf, &endpos, 10);
            if (endpos == buf)
                break;
            buf = endpos;
            long source_range = strtol(buf, &buf, 10);
            long range_len = strtol(buf, &buf, 10);
            printf("from %ld to %ld (size %ld)\n", source_range, target_range, range_len);
            for (int i = 0; i < seed_el_count; i++) {
                const long el = arr1[i];
                if ((source_range <= el) && (el < source_range + range_len)) {
                    arr2[i] = el + target_range - source_range;
                    arr1[i] = -1;
                }
            }
        }
        for (int i = 0; i < seed_el_count; i++) {
            if (arr1[i] == -1)
                arr1[i] = arr2[i];
        }

        long *tmp = arr1;
        arr1 = arr2;
        arr2 = tmp;
//        print_arr(arr1, seed_el_count);
        strsep(&buf, "\n"); //skip newline after last number
        strsep(&buf, "\n"); // skip empty line
    }
    long min_el=arr1[0];
    for (int i = 0; i < seed_el_count; i++) {
        min_el=min(min_el,arr1[i]);
    }
    day_res->part1_result=min_el;

    return day_res;
}

void solve_day05() {
    struct two_part_result *day_res = allocate_two_part_result();
    char *input_buffer;
    // harder to parallelize b.c. of arbitrary range interactions!
    const long filesize = read_file_to_memory(DAY05_FILE, &input_buffer, false);
    day_res = day05(input_buffer, filesize);
    print_day_result("day05", day_res);
    free_two_part_result(day_res);
    free(input_buffer);
}

#endif // AOCC_DAY05_H
