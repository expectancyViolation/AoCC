#include <stdio.h>
#include <string.h>

#define CHECK_COUNT 20

const char *digits[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
const char *digit_and_names[20] = {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "zero",
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine"
};
const int lens[20]= {1,1,1,1,1,1,1,1,1,1,4,3,3,5,4,4,3,5,5,4};

int get_first(char* buffer){
    for(int i=0;buffer[i]!=0;i++){
        for(int j=0;j<CHECK_COUNT;j++){
            if(strncmp(buffer+i,digit_and_names[j],lens[j])==0){
                return j%10;
            }
        }
    }
}

int get_last(char* buffer){
    for(int i= strlen(buffer);i>=0;i--){
        for(int j=0;j<CHECK_COUNT;j++){
            if(strncmp(buffer+i,digit_and_names[j],lens[j])==0){
                return j%10;
            }
        }
    }
}

int main() {
    FILE *input;
    const int buflen = 1000;
    char buffer[buflen];
    int res=0;
    input = fopen("/tmp/day01_bigboy", "r");
    while (fgets(buffer, buflen, input) != NULL) {
        const int curr=get_first(buffer)*10+ get_last(buffer);
        res+= curr;
    }
    printf("%d",res);
    return 0;
}
