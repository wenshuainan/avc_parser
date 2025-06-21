 #ifndef __AVC_PARSE_H__
 #define __AVC_PARSE_H__


typedef void* avc_parse_t;

typedef int (*frame_callback_t)(unsigned char *frame, int size);


int avc_parse_create(int max_size, frame_callback_t callback, avc_parse_t *obj);

int avc_parse_destroy(avc_parse_t *obj);

int avc_parse_put_data(avc_parse_t obj, unsigned char *data, int size);


#endif