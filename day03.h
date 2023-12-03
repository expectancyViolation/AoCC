//
// Created by matze on 03/12/2023.
//

#ifndef AOCC_DAY03_H
#define AOCC_DAY03_H

#include "two_part_result.h"
#include <collectc/cc_hashtable.h>

void day03_place_pad_line(char *buf, long line_length) {
    memset(buf, '.', line_length);
    *(buf + line_length - 1) = '\n';
}

long day03_pad_input(char *input_buffer, char **padded_buffer_out,
                     long filesize) {
    const long line_length = get_first_line_length(input_buffer);
    const long padded_buffer_len = filesize + 4 * (line_length) - 1;
    *padded_buffer_out = malloc(padded_buffer_len);
    char *padded_buffer = *padded_buffer_out;
    char *pad_ptr = padded_buffer;
    day03_place_pad_line(pad_ptr, line_length);
    pad_ptr += line_length;
    day03_place_pad_line(pad_ptr, line_length);
    pad_ptr += line_length;
    strncpy(pad_ptr, input_buffer, filesize - 1);
    pad_ptr += filesize - 1;
    day03_place_pad_line(pad_ptr, line_length);
    pad_ptr += line_length;
    day03_place_pad_line(pad_ptr, line_length);
    *(padded_buffer + padded_buffer_len - 1) = 0;
    return padded_buffer_len;
}

bool is_dig(char c) { return (c >= '0') && (c <= '9'); }

int day03_parse_numbers(char **curr_number_pos, char **curr_number_end) {
    while ((!is_dig(**curr_number_pos)) && (**curr_number_pos != 0))
        ++*curr_number_pos;
    assert(**curr_number_pos != '0'); // no leading zeros (and neither 0)

    if (**curr_number_pos == 0)
        return 0;
    const int number = (int) strtol(*curr_number_pos, curr_number_end, 10);
    return number;
}

bool day03_is_symbol(char c) {
    return ((c != '.') && (c != 0) && (c != '\n') && !is_dig(c));
}

bool day03_is_adjacent_to_symbol(char *segment_begin, const char *segment_end,
                                 long line_length) {
    bool result = false;
    for (int i = -1; i < 2; i++) {
        const long offset = i * line_length;
        for (char *ptr = segment_begin - 1; ptr != segment_end + 1; ++ptr) {
            result |= day03_is_symbol(*(ptr + offset));
        }
    }
    return result;
}

void increment_count_table(CC_HashTable *count_table, const char *pos) {
    long *cnt;
    enum cc_stat got = cc_hashtable_get(count_table, pos, (void **) &cnt);
    if (got != CC_OK) {
        cnt = malloc(sizeof *cnt);
        *cnt = 0;
    }
    (*cnt) += 1;
    cc_hashtable_add(count_table, pos, cnt);
}

void multiply_prod_table(CC_HashTable *prod_table, const char *pos, long val) {
    long *cnt;
    enum cc_stat got = cc_hashtable_get(prod_table, pos, (void **) &cnt);
    if (got != CC_OK) {
        cnt = malloc(sizeof *cnt);
        *cnt = 1;
    }
    (*cnt) *= val;
    cc_hashtable_add(prod_table, pos, cnt);
}


void day03_mark_gears(char *segment_begin, const char *segment_end,
                      long line_length, long value, CC_HashTable *count_table, CC_HashTable *prod_table) {
    for (int i = -1; i < 2; i++) {
        const long offset = i * line_length;
        for (char *ptr = segment_begin - 1; ptr != segment_end + 1; ++ptr) {
            const char *curr_pos = (ptr + offset);
            if (*curr_pos == '*') {
                increment_count_table(count_table, curr_pos);
                multiply_prod_table(prod_table, curr_pos, value);
            }
        }
    }
}

struct two_part_result *day03(char *buf, long buf_len) {
    struct two_part_result *result = allocate_two_part_result();
    const long line_length = get_first_line_length(buf);

    char *curr_number_pos = buf;
    char *part2_segment_begin = buf + line_length;
    char *part1_segment_begin = curr_number_pos + 2 * line_length;
    char *curr_number_end;

    char *part1_segment_end = buf + buf_len - 2 * line_length;
    char *part2_segment_end = buf + buf_len - 1 * line_length;


    int curr_number = -1;
    CC_HashTable *count_table;
    CC_HashTable *prod_table;



    if (cc_hashtable_new(&count_table) != CC_OK) {
        printf("failed to create hashtable");
    }
    if (cc_hashtable_new(&prod_table) != CC_OK) {
        printf("failed to create hashtable");
    }
    while (true) {
        curr_number = day03_parse_numbers(&curr_number_pos, &curr_number_end);
        if (curr_number == 0)
            break;
        const bool is_adjacent = day03_is_adjacent_to_symbol(
                curr_number_pos, curr_number_end, line_length);

        //day03_mark_gears(curr_number_pos, curr_number_end, line_length, curr_number, count_table, prod_table);

        if ((part1_segment_begin <= curr_number_pos) && (curr_number_pos < part1_segment_end)) {
            if (is_adjacent) {
                result->part1_result += curr_number;
            }
        }
        curr_number_pos = curr_number_end;
    }

    CC_HashTableIter hti;

    cc_hashtable_iter_init(&hti, count_table);
    TableEntry *entry;
    long *gear_val;
    while (cc_hashtable_iter_next(&hti, &entry) != CC_ITER_END) {
        long *val = entry->value;
        char *gear_pos = entry->key;
        if ((*val) == 2) {
            if ((part1_segment_begin <= gear_pos) && (gear_pos <= part1_segment_end)) {
                cc_hashtable_get(prod_table, entry->key, (void **) &gear_val);
                result->part2_result += *gear_val;
            }
        }
    }
    return result;
}

#endif // AOCC_DAY03_H
