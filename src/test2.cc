#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */

int main(int argc, char *argv[]) {
    uint16_t t1 = 23;
    uint16_t t2 = ((t1 + 7)/8)*8;
    printf("%d -> %d\r\n", t1, t2);
}