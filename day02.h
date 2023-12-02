//
// Created by matze on 02/12/2023.
//

#include "helper.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct draw {
    int red;
    int green;
    int blue;
};
static const struct draw EMPTY_DRAW;

bool draw_is_leq(const struct draw *draw1, const struct draw *draw2) {
    return (draw1->red <= draw2->red) && (draw1->green <= draw2->green) &&
           (draw1->blue <= draw2->blue);
}

void draw_set_max(struct draw *draw_to_set, const struct draw *draw_other) {
    draw_to_set->red = max(draw_to_set->red, draw_other->red);
    draw_to_set->green = max(draw_to_set->green, draw_other->green);
    draw_to_set->blue = max(draw_to_set->blue, draw_other->blue);
}

void print_draw(const struct draw *draw_) {
    printf("----\ndraw:\n r = %d\n g = %d\n b = %d\n", draw_->red, draw_->green,
           draw_->blue);
}

int draw_power(const struct draw *draw_) {
    return (draw_->red) * (draw_->blue) * (draw_->green);
}

void advance_space(char **buf_pos) {
    while (**buf_pos == ' ')
        ++*buf_pos;
}

void get_next_draw(char **buf_pos, struct draw *next_draw) {
    *next_draw = EMPTY_DRAW;
    char *draw = strsep(buf_pos, ";");
    char **draw_pos = &draw;
    while (*draw_pos != NULL) {
        const int count = (int) strtol(*draw_pos, draw_pos, 10);
        advance_space(draw_pos);
        char *color = strsep(draw_pos, ",");
        // assume that the only colors are "red","green","blue" => just check first
        // letter
        switch (color[0]) {
            case 'r':
                next_draw->red = count;
                break;
            case 'g':
                next_draw->green = count;
                break;
            case 'b':
                next_draw->blue = count;
                break;
        }
    }
}

long parse_game_id(char **buf_pos) {
    *buf_pos += 4;
    const long game_id = strtol(*buf_pos, buf_pos, 10);
    *buf_pos += 1;
    return game_id;
}

void get_required_draw(char *buf, struct draw *draw_limit, long *game_id_out,
                       struct draw *required_draw_out) {
    struct draw current_draw;
    char **buf_pos = &buf;
    *game_id_out = parse_game_id(buf_pos);
    while (*buf_pos != NULL) {
        get_next_draw(buf_pos, &current_draw);
        draw_set_max(required_draw_out, &current_draw);
    }
}

long long day02(char *buf) {
    struct draw draw_limit = {12, 13, 14};
    struct draw required_draw;
    long game_id = -1;
    char **buf_pos = &buf;
    long long possible_game_ids_sum = 0;
    long long total_power = 0;
    int cnts = 0;
    while (*buf_pos != NULL) {
        cnts += 1;
        char *line = strsep(buf_pos, "\n");
        required_draw = EMPTY_DRAW;
        get_required_draw(line, &draw_limit, &game_id, &required_draw);
        const bool game_ok = draw_is_leq(&required_draw, &draw_limit);
        if (game_ok) {
            possible_game_ids_sum += game_id;
        }
        total_power += draw_power(&required_draw);
    }
    return total_power;
}
