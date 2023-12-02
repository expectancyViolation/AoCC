#include <stdio.h>
#include <string.h>

// part 1
//# define CHECK_COUNT 10
// part 2
#define CHECK_COUNT 20

const char *digit_and_names[20] = {"0", "1", "2", "3", "4",
                                   "5", "6", "7", "8", "9",
                                   ".", "one", "two", "three", "four",
                                   "five", "six", "seven", "eight", "nine"};
const long long masks[20] = {255, 255, 255, 255,
                             255, 255, 255, 255,
                             255, 255, 255, 16777215,
                             16777215, 1099511627775, 4294967295, 4294967295,
                             16777215, 1099511627775, 1099511627775, 4294967295};

//check if "digit" starts at position j in buffer buff_int
int matches(const long long *buff_int, const int j) {
    const long long *dig_int = (long long*)(digit_and_names[j]);
    const long long symdiff = (*buff_int) ^ (*dig_int);
    return !((symdiff) & (masks[j]));

}

int get_first(const char *buffer) {
    for (int i = 0; buffer[i] != 0; i++) {
        for (int j = 0; j < CHECK_COUNT; j++) {
            if (matches((long long*)(buffer+i), j)) {
                return j % 10;
            }
        }
    }
}

int get_last(const char *buffer) {
    for (int i = strlen(buffer); i >= 0; i--) {
        for (int j = 0; j < CHECK_COUNT; j++) {
            if (matches((long long*)(buffer+i), j)) {
                return j % 10;
            }
        }
    }
}

long long day01(char* buf) {
    long long res = 0;
    char** buf_pos=&buf;
    while (*buf_pos != NULL) {
        char* line = strsep(buf_pos,"\n");
        res += get_first(line) * 10 + get_last(line);
    }
    return res;
}