#include "cvector.h"
#include "fenwick.h"
#include "helpers.h"
#include "parallelize.h"
#include "two_part_result.h"
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#ifndef AOCC_DAY05_H
#define AOCC_DAY05_H

#define DAY05_FILE "/tmp/day05_bigboy"

struct range {
    long begin;
    long end;
};

struct mapping {
    struct range r;
    long long offset;
};

bool range_is_empty(const struct range *r) { return (r->begin) >= (r->end); }

int range_compare_lex(const struct range *r1, const struct range *r2) {
    const long diff_begin = (r1->begin) - (r2->begin);
    const long diff_end = (r1->end) - (r2->end);
    // prevent compare function overflow
    return (diff_begin != 0) ? (diff_begin > 0) - (diff_begin < 0)
                             : (diff_end > 0) - (diff_end < 0);
}


int mapping_compare_lex(const struct mapping *m1, const struct mapping *m2) {
    return range_compare_lex(&m1->r, &m2->r);
}

void range_intersect(const struct range *r1, const struct range *r2,
                     struct range *result) {
    result->begin = max(r1->begin, r2->begin);
    result->end = min(r1->end, r2->end);
}

void shift_range(struct range *r, long offset) {
    r->begin += offset;
    r->end += offset;
}

bool range_fully_contains(const struct range *r1, const struct range *r2) {
    return (r1->begin < r2->begin) && (r2->end < r1->end);
}

__attribute__((unused)) void parse_seeds_p1(char *line, struct range **vec) {
    struct range parsed_range;
    strsep(&line, ":");
    char *endpos;
    while (true) {
        const long curr_num = strtol(line, &endpos, 10);
        if (endpos == line)
            break;
        line = endpos;
        parsed_range.begin = curr_num;
        parsed_range.end = curr_num + 1;
        cvector_push_back(*vec, parsed_range);
    }
    printf("donso\n");
}

void parse_seeds_p2(char *line, struct range **vec) {
    struct range parsed_range;
    strsep(&line, ":");
    char *endpos;
    while (true) {
        parsed_range.begin = strtol(line, &endpos, 10);
        parsed_range.end = parsed_range.begin;
        if (endpos == line)
            break;
        line = endpos;
        parsed_range.end += strtol(line, &line, 10);
        cvector_push_back(*vec, parsed_range);
    }
}

void print_range(const struct range *r) {
    printf("range from %ld to %ld", r->begin, r->end);
}

__attribute__((unused)) void print_seed_arr(const struct range *arr) {
    for (size_t i = 0; i < cvector_size(arr); i++) {
        print_range(&arr[i]);
        printf("\n");
    }
    printf("\n");
}

long long seed_arr_sum(const struct range *arr){
    long long res=0;
    for (size_t i = 0; i < cvector_size(arr); i++) {
        assert(arr[i].end>arr[i].begin);
        res+=arr[i].end-arr[i].begin;
    }
    return res;
}

void print_mappings(const struct mapping *arr) {
    for (size_t i = 0; i < cvector_size(arr); i++) {
        print_range(&arr[i].r);
        printf(" step: %lld\n",arr[i].offset);
    }
    printf("\n");
}

void coalesce_seed_arr(struct range **v, struct range **out) {
//    for (size_t i = 1; i < cvector_size(*v); i++) {
//        struct range *el = (*v) + i;
//        assert(el->begin < el->end);
//    }
    struct range temp_range;
    temp_range.begin = (*v)[0].begin;
    temp_range.end = (*v)[0].end;
    for (size_t i = 1; i < cvector_size(*v); i++) {
        const struct range *curr_range = &(*v)[i];
        if (curr_range->begin > temp_range.end) {
            // temp_range closes
            cvector_push_back(*out, temp_range);
            temp_range.begin = curr_range->begin;
            temp_range.end = curr_range->end;
        } else {
            // coalesce
            temp_range.end = max(temp_range.end,curr_range->end);
        }
    }
    cvector_push_back(*out, temp_range);
}

struct two_part_result *day05(char *buf,
                              __attribute__((unused)) long _buf_len) {
    struct two_part_result *day_res = allocate_two_part_result();
    char *line = strsep(&buf, "\n");
    struct range *source = NULL;
    struct range *dest = NULL;
    struct range parsed_source_range;
    struct range temp_range;
    struct mapping temp_mapping;
    struct mapping *mappings = NULL;
    struct mapping *filled_mappings = NULL;

    parse_seeds_p2(line, &source);

    qsort(source, cvector_size(source), sizeof(struct range),
          (__compar_fn_t) range_compare_lex);
    //print_seed_arr(source);
    strsep(&buf, "\n"); // remove first newline
    while (true) {
        if (buf == NULL)
            break;
        strsep(&buf, "\n"); // discard map title
        char *end_pos;
        cvector_clear(mappings);
        while (true) {
            long destination_range_begin = strtol(buf, &end_pos, 10);
            if (end_pos == buf)
                break;
            buf = end_pos;
            long source_range_begin = strtol(buf, &buf, 10);
            long range_len = strtol(buf, &buf, 10);
            temp_mapping.r.begin = source_range_begin;
            temp_mapping.r.end = source_range_begin + range_len;
            temp_mapping.offset = destination_range_begin - source_range_begin;
            cvector_push_back(mappings, temp_mapping);
        }
        qsort(mappings, cvector_size(mappings), sizeof(struct mapping),
              (__compar_fn_t) mapping_compare_lex);


        const struct mapping *m_it;
        const struct range *r_it;
        temp_mapping.r.begin = LLONG_MIN>>2;
        temp_mapping.r.end = LLONG_MIN>>2;
        temp_mapping.offset = 0;
        cvector_clear(filled_mappings);
        for (m_it = cvector_begin(mappings); m_it != cvector_end(mappings); ++m_it) {
            if (m_it->r.begin > temp_mapping.r.end) {
                temp_mapping.r.begin = temp_mapping.r.end;
                temp_mapping.r.end = m_it->r.begin;
                temp_mapping.offset = 0;
                cvector_push_back(filled_mappings, temp_mapping);
            }
            temp_mapping.r.begin = m_it->r.begin;
            temp_mapping.r.end = m_it->r.end;
            temp_mapping.offset = m_it->offset;
            cvector_push_back(filled_mappings, temp_mapping);
        }
        temp_mapping.r.begin = temp_mapping.r.end;
        temp_mapping.r.end = LLONG_MAX>>2;
        temp_mapping.offset = 0;

        cvector_push_back(filled_mappings, temp_mapping);

        qsort(filled_mappings, cvector_size(filled_mappings), sizeof(struct mapping),
              (__compar_fn_t) mapping_compare_lex);
        //print_mappings(filled_mappings);
        //printf("-------------");
        cvector_clear(dest);
        for (m_it = cvector_begin(filled_mappings); m_it != cvector_end(filled_mappings); ++m_it) {
            for (r_it = cvector_begin(source); r_it != cvector_end(source); ++r_it) {
                range_intersect(&(m_it->r), r_it, &temp_range);
//                printf("intersecting: ");
//                print_range(&(m_it->r));
//                printf("\nwith ");
//                print_range(r_it);
//                printf("\n");
//                print_range(&temp_range);
//                printf("\nshifted:\n");
                shift_range(&temp_range,m_it->offset);
                //print_range(&temp_range);
                //printf("\n");
                if (!range_is_empty(&temp_range)) {
//                    printf("PUSHING!!!!!!!!!!!!!!!!!!!!!!!\n\n");
                    cvector_push_back(dest, temp_range);
                }
            }
        }
        cvector_clear(source);
        //coalesce_seed_arr(&dest, &source);
//        struct range* temp=source;
//        source=dest;
//        dest=temp;
//
//        qsort(source, cvector_size(source), sizeof(struct range),
//              (__compar_fn_t) range_compare_lex);

        const long long sar_before=seed_arr_sum(dest);
        //printf("SAR BEFORE IS:%lld\n",sar_before);
        qsort(dest, cvector_size(dest), sizeof(struct range),
              (__compar_fn_t) range_compare_lex);
        coalesce_seed_arr(&dest, &source);
        //const long long sar_after=seed_arr_sum(source);
        //printf("SAR IS:%lld\n",sar_after);

        strsep(&buf, "\n"); // skip newline after last number
        strsep(&buf, "\n"); // skip empty line
    }
    long long min_el = 10000000000;
    for (size_t i = 0; i < cvector_size(source); i++) {
        if (source[i].end > source[i].begin)
            min_el = min(min_el, source[i].begin);
    }
    day_res->part2_result = min_el;

    cvector_free(source);
    cvector_free(dest);
    return day_res;
}

void solve_day05() {
    struct two_part_result *day_res;
    char *input_buffer;
    // harder to parallelize b.c. of arbitrary range interactions!
    const long filesize = read_file_to_memory(DAY05_FILE, &input_buffer, false);
    day_res = day05(input_buffer, filesize);
    print_day_result("day05", day_res);
    free_two_part_result(day_res);
    free(input_buffer);
}

#endif // AOCC_DAY05_H
