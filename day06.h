//
// Created by matze on 06/12/2023.
//

#ifndef AOCC_DAY06_H
#define AOCC_DAY06_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "helpers.h"
#include "two_part_result.h"

#define DAY06_FILE "/tmp/day06"
// to nudge floating point results in correct direction
#define EPS 0.00000000001

//from: https://stackoverflow.com/questions/1726302/remove-spaces-from-a-string-in-c
void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

long long solve(char* buf){
    char* line1=strsep(&buf,"\n");
    char* line2=buf;
    strsep(&line1,":");
    strsep(&line2,":");
    char* endpos;
    long long res=1;
    while(true){
        const double time = (double)(strtol(line1, &endpos, 10));
        if (endpos == line1)
            break;
        line1=endpos;
        const double distance = (double)strtol(line2, &line2, 10);
        printf("race %f %f\n",time,distance);

        // solve quadratic equation x*(time-x)=distance;
        const long disc=time*time-4*distance;
        const long lower=ceil(((time- sqrt(disc))/2)+EPS);
        const long upper=floor((time+ sqrt(disc))/2-EPS);
        printf("range: %ld %ld\n\n",lower,upper);
        res*=(upper-lower+1);
    }
    return res;
}

struct two_part_result *day06(char *buf,
                              long buf_len) {
    struct two_part_result *day_res= malloc(sizeof *day_res);
    const size_t buf_size=buf_len*sizeof(*buf);
    char * buf_copy= malloc(buf_size);
    memcpy(buf_copy,buf,buf_size);
    day_res->part1_result=solve(buf_copy);
    remove_spaces(buf);
    day_res->part2_result=solve(buf);
    return day_res;
}

void solve_day06() {
    struct two_part_result *day_res;
    char *input_buffer;
    const long filesize = read_file_to_memory(DAY06_FILE, &input_buffer, false);
    day_res = day06(input_buffer, filesize);
    print_day_result("day06", day_res);
    free_two_part_result(day_res);
    free(input_buffer);
}

#endif //AOCC_DAY06_H
