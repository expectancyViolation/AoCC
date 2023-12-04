//
// Created by matze on 03/12/2023.
//

#ifndef AOCC_DAY03_H
#define AOCC_DAY03_H

#include "two_part_result.h"
#include "cvector.h"

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

struct day03_gear_match {
    char *pos;
    long num;
};

long compare_day03_gear_match(struct day03_gear_match *m1, struct day03_gear_match *m2) {
    return (m1->pos) - (m2->pos);
}

void day03_mark_gears(char *segment_begin, const char *segment_end,
                      long line_length, long value, struct day03_gear_match **vec) {
    struct day03_gear_match match;
    match.num = value;
    for (int i = -1; i < 2; i++) {
        const long offset = i * line_length;
        for (char *ptr = segment_begin - 1; ptr != segment_end + 1; ++ptr) {
            const char *curr_pos = (ptr + offset);
            if (*curr_pos == '*') {
                match.pos = curr_pos;
                cvector_push_back(*vec, match);
            }
        }
    }
}


struct two_part_result *day03(char *buf, long buf_len) {
    struct two_part_result *result = allocate_two_part_result();
    const long line_length = get_first_line_length(buf);

    char *curr_number_pos = buf;
    char *part1_segment_begin = curr_number_pos + 2 * line_length;
    char *curr_number_end;

    char *part1_segment_end = buf + buf_len - 2 * line_length;

    struct day03_gear_match *v = NULL;
    //struct day03_gear_match m={NULL,65};
    //cvector_push_back(v,m);
    int curr_number = -1;
    while (true) {
        curr_number = day03_parse_numbers(&curr_number_pos, &curr_number_end);
        if (curr_number == 0)
            break;
        const bool is_adjacent = day03_is_adjacent_to_symbol(
                curr_number_pos, curr_number_end, line_length);

        day03_mark_gears(curr_number_pos, curr_number_end, line_length, curr_number, &v);

        if ((part1_segment_begin <= curr_number_pos) && (curr_number_pos < part1_segment_end)) {
            if (is_adjacent) {
                result->part1_result += curr_number;
            }
        }
        curr_number_pos = curr_number_end;
    }
    qsort(v, cvector_size(v), sizeof (struct day03_gear_match), (__compar_fn_t) compare_day03_gear_match);

    char *prev_pos = NULL;
    int curr_count = 0;
    long curr_prod = 1;
    if (v) {
        struct day03_gear_match *it;
        int i = 0;
        for (it = cvector_begin(v); it != cvector_end(v); ++it) {
            const char *curr_pos = it->pos;
            if (curr_pos != prev_pos) {
                if (curr_count == 2) {
                    if((part1_segment_begin <= prev_pos) && (prev_pos < part1_segment_end) )
                    result->part2_result += curr_prod;
                }
                curr_count = 0;
                curr_prod = 1;
            }
            curr_count += 1;
            curr_prod *= it->num;
            prev_pos = curr_pos;
        }
        if (curr_count == 2) {
            if((part1_segment_begin <= prev_pos) && (prev_pos < part1_segment_end) )
            result->part2_result += curr_prod;
        }
    }


    return  result;
}


#endif // AOCC_DAY03_H
