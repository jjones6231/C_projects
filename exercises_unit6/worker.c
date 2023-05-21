#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

//shifts uint a by n bits, appending the bit
//shifted off to the least significant bit position
uint32_t circular_shift(uint32_t a, uint32_t n){
    for(int i = 0; i < n; ++i)
        a = (a << 1) | (a >> 31);
    return a;
}

//takes a uint and prints the binary value of it
void int_to_binary(uint32_t dec_num){
    printf("binary value of 0x%x: ", dec_num);
    for(int i = 0; i< sizeof(dec_num); ++i){
        for(int j = 0; j < 8; j++){
            printf("%d",(dec_num >> ((sizeof(dec_num)-(i*8+j))) & 1));
        }
    }
    printf("\n");
}

//packs 4 bytes into a string the order they are given
uint32_t pack_bytes(char a, char b, char c, char d){
    uint32_t res = 0;
    res = (res << 8) | a;
    res = (res << 8) | b;
    res = (res << 8) | c;
    res = (res << 8) | d;
    return res;
}

//run some tests
int main(int argc, char *argv[]){

    uint32_t working = 0xdead;
    printf("shifted value: 0x%x\n", circular_shift(working, 4));
    printf("shifted value: 0x%x\n", circular_shift(working, 16));
    printf("shifted value: 0x%x\n", circular_shift(working, 28));

    int_to_binary(working);
    int_to_binary(0xf00dd00d);
    int_to_binary(0x9090);

    working = pack_bytes('C', 'H', 'E', 'F');

    printf("working integer: 0x%x\n", working);
    printf("working string: %s\n", &working);


    return 0;
}

