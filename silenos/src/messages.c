#include "messages.h"
#include "cbor.h"

#include "stdio.h"

int encode_data(uint8_t *buf, size_t buf_size, int *data, int data_len, int event_counter, int seq_num)
{
    int ret = 0;

    CborEncoder encoder, arrayEncoder, mapEncoder;
    /* init and create map */
    cbor_encoder_init(&encoder, buf, buf_size, 0);
    ret |= cbor_encoder_create_map(&encoder, &mapEncoder, 3);

    /* create key 'd' data array */
    ret |= cbor_encode_text_stringz(&mapEncoder, "d");
    ret |= cbor_encoder_create_array(&mapEncoder, &arrayEncoder, data_len);
    for (int i = 0; i < data_len; ++i)
    {
        ret |= cbor_encode_int(&arrayEncoder, data[i]);
    }
    ret |= cbor_encoder_close_container(&mapEncoder, &arrayEncoder);

    /* create key 'c' for event counter */
    ret |= cbor_encode_text_stringz(&mapEncoder, "c");
    ret |= cbor_encode_int(&mapEncoder, event_counter);
    /* create key 's' sequence number */
    ret |= cbor_encode_text_stringz(&mapEncoder, "s");
    ret |= cbor_encode_int(&mapEncoder, seq_num);

    ret |= cbor_encoder_close_container(&encoder, &mapEncoder);

    if (ret != 0)
    {
        printf("Failed to encode data: %s \n", cbor_error_string(ret));
        return -1;
    }
    return 0;
}


void send_data(uint8_t *cbor_buf, size_t buf_size)
{
    (void)cbor_buf;
    (void)buf_size;
    // lora_send_data(cbor_buf, buf_size);
}
