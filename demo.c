#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "avc_parse.h"


static int frame_callback(unsigned char *frame, int size)
{
    printf(">>>>>> %0#x, %0#x, %0#x, %0#x, %0#x\n", frame[0], frame[1], frame[2], frame[3], frame[4]);

    /* code */

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2 || argv[1] == NULL || strlen(argv[1]) == 0) {
        printf("useage: ./a.out file.avc\n");
        return -1;
    }

    avc_parse_t praser = NULL;
    int ret = avc_parse_create(1048576, frame_callback, &praser);
    if (ret != 0) {
        printf("avc_parse_create\n");
        return -1;
    }

    FILE *h264 = fopen(argv[1], "rb");
    if (h264 == NULL) {
        printf("fopen\n");
        return -1;
    }

    unsigned char data[1024];
    int read_size = 0;
    while ((read_size = fread(data, 1, sizeof(data), h264)) > 0)
    {
        avc_parse_put_data(praser, data, read_size);
    }

    avc_parse_destroy(&praser);
    
    return 0;
}
