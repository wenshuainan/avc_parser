#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "avc_parse.h"


typedef struct {
    int                 max_size;
    unsigned char       *buf;
    int                 size;
    frame_callback_t    callback;
} parse_t;


static int is_start_code(unsigned char *data)
{
    if (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x01) {
        return 3;
    } else if (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x00 && data[3] == 0x01){
        return 4;
    } else {
        return 0;
    }
}

int avc_parse_create(int max_size, frame_callback_t callback, avc_parse_t *obj)
{
    if (max_size <= 0 || callback == NULL || obj == NULL) {
        return -1;
    }

    *obj = (parse_t *)malloc(sizeof(parse_t));
    if (*obj == NULL) {
        return -1;
    }

    parse_t *proxy = (parse_t *)(*obj);
    proxy->buf = (unsigned char *)malloc(max_size + 64);
    if (proxy->buf == NULL) {
        free(*obj);
        return -1;
    }

    proxy->max_size = max_size + 64;
    proxy->size = 0;
    proxy->callback = callback;

    return 0;
}

int avc_parse_destroy(avc_parse_t *obj)
{
    if (obj == NULL || *obj == NULL) {
        return -1;
    }

    parse_t *proxy = (parse_t *)(*obj);
    free(proxy->buf);

    free(*obj);
    *obj = NULL;

    return 0;
}

int avc_parse_put_data(avc_parse_t obj, unsigned char *data, int size)
{
    if (obj == NULL || data == NULL || size <= 0) {
        return -1;
    }

    parse_t *proxy = (parse_t *)obj;

    while (size > 0)
    {
        int left = proxy->max_size - proxy->size;
        int cp_size = left > size ? size : left;
        memcpy(proxy->buf + proxy->size, data, cp_size);
        proxy->size += cp_size;
        size -= cp_size;
        data += cp_size;

        if (proxy->size >= proxy->max_size) {
            int i = 2;  /* escape start code 0x00 0x00 */
            for (; i < proxy->size - 3; i++)
            {
                if (is_start_code(proxy->buf + i)) {
                    if (is_start_code(proxy->buf)) {
                        int ret = proxy->callback(proxy->buf, i);
                        if (ret != 0) {
                            return ret;
                        }
                    }

                    memmove(proxy->buf, proxy->buf + i, proxy->size - i);
                    proxy->size -= i;
                    break;
                }
            }

            /* not found */
            if (i == proxy->size - 3) {
                memmove(proxy->buf, proxy->buf + i, 3);
                proxy->size -= i;
            }
        }
    }

    return 0;
}
