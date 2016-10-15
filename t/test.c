#include <stdio.h>
#include <stdlib.h>

#define FINGER_TBL_SIZE 16

typedef unsigned long node_id_t;


int main()
{
    node_id_t init = 100000, step = 1;
    int i = 0, interval = (sizeof(node_id_t) * 8) / FINGER_TBL_SIZE;

    printf("Interval = %d\n", interval);
    for(; i < FINGER_TBL_SIZE; i++) {
        node_id_t start = init + (step << (i * interval));
        printf("entry[%d] = %lu (shift is %d)\n", i, start, (i*interval));
    }
}

