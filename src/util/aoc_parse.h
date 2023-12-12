#ifndef AOCC_AOC_PARSE_H
#define AOCC_AOC_PARSE_H

#include "aoc_types.h"

#include "helpers.h"

#include <stdbool.h>

// parse
bool parse_day_status(char *raw_response, AocDayStatus *out);

bool parse_submission_status(char *raw_response, AocSubmissionStatus *out);

// generate
char *get_answer_url(int year, int day);
char *get_input_url(int year, int day);
char *get_status_url(int year, int day);
char *format_answer_payload(enum AOC_DAY_PART part, const AocPartRes *guess);

void test_aoc_parse();

#endif // AOCC_AOC_PARSE_H
