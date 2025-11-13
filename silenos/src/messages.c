#include "messages.h"
#include "lora_networking.h"
#include "cbor.h"

#include "sensors.h"
#include "state_validation.h"
#include "stdio.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

static uint16_t cbor_buf_size = 0;
// static uint8_t cbor_buf[CBOR_BUFFER_SIZE] = { 0 };
static uint8_t *cbor_buf;

static int seq_num = 0;

int encode_data(uint8_t *buf,
                size_t buf_size,
                gate_state_t gate_state,
                int seq_num,
                uint32_t timestamp_s)
{
    int ret = 0;

    CborEncoder encoder, array_encoder_i, array_encoder_v, array_encoder_c, map_encoder;
    /* init and create map */
    cbor_encoder_init(&encoder, buf, buf_size, 0);
    ret |= cbor_encoder_create_map(&encoder, &map_encoder, CBOR_MSG_MAP_LENGTH); /* 10 bytes */

    /* create key 'l1' location part 1 - polder */
    ret |= cbor_encode_text_stringz(&map_encoder, "l1"); /* 2 bytes */
    ret |= cbor_encode_uint(&map_encoder, SENSOR_CONFIG_LOCATION_POLDER);

    /* create key 'l2' location part 2 - gate */
    ret |= cbor_encode_text_stringz(&map_encoder, "l2"); /* 2 bytes */
    ret |= cbor_encode_uint(&map_encoder, SENSOR_CONFIG_LOCATION_GATE);

     /* create key 'ib' id bits   */
    ret |= cbor_encode_text_stringz(&map_encoder, "ib"); /* 2 bytes */
    ret |= cbor_encode_uint(&map_encoder, SENSOR_ENCODE_SENSOR_ID_BITS);

    /* create key 'tb' type bits   */
    ret |= cbor_encode_text_stringz(&map_encoder, "tb"); /* 2 bytes */
    ret |= cbor_encode_uint(&map_encoder, SENSOR_ENCODE_TYPE_BITS);

        /* create key 'vb' id bits   */
    ret |= cbor_encode_text_stringz(&map_encoder, "vb"); /* 2 bytes */
    ret |= cbor_encode_uint(&map_encoder, SENSOR_ENCODE_VALUE_ID_BITS);

    /* create key 'i' identifier array */
    ret |= cbor_encode_text_stringz(&map_encoder, "i"); /* 1 byte */
    ret |= cbor_encoder_create_array(&map_encoder, &array_encoder_i, NUM_UNIQUE_SENSOR_VALUES);

    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; ++i) {
        sensor_value_state_t sensor = gate_state.sensor_states[i];
        ret |= cbor_encode_uint(&array_encoder_i, ENCODE_SENSOR_TYPE_IDS(sensor.sensor_id,sensor.type, sensor.value_id));
    }
    ret |= cbor_encoder_close_container(&map_encoder, &array_encoder_i);

    /* create key 'v' value array */
    ret |= cbor_encode_text_stringz(&map_encoder, "v"); /* 1 byte */
    ret |= cbor_encoder_create_array(&map_encoder, &array_encoder_v, NUM_UNIQUE_SENSOR_VALUES);
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; ++i) {
        sensor_value_state_t sensor = gate_state.sensor_states[i];

        ret |= cbor_encode_uint(&array_encoder_v, sensor.value);
    }
    ret |= cbor_encoder_close_container(&map_encoder, &array_encoder_v);

    /* create key 'c' event counter array */
    ret |= cbor_encode_text_stringz(&map_encoder, "c"); /* 1 byte */
    ret |= cbor_encoder_create_array(&map_encoder, &array_encoder_c, NUM_UNIQUE_SENSOR_VALUES);

    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; ++i) {
        sensor_value_state_t sensor = gate_state.sensor_states[i];

        ret |= cbor_encode_uint(&array_encoder_c, sensor.event_counter);
    }
    ret |= cbor_encoder_close_container(&map_encoder, &array_encoder_c);

    /* create key 'g' gate state */
    ret |= cbor_encode_text_stringz(&map_encoder, "g"); /* 1 byte */
    ret |= cbor_encode_boolean(&map_encoder, gate_state.state);

    /* create key 's' sequence number */
    ret |= cbor_encode_text_stringz(&map_encoder, "s"); /* 1 byte */
    ret |= cbor_encode_uint(&map_encoder, seq_num);

    /* create key 't' timestamp number */
    ret |= cbor_encode_text_stringz(&map_encoder, "t"); /* 1 byte */
    ret |= cbor_encode_uint(&map_encoder, timestamp_s);

    ret |= cbor_encoder_close_container(&encoder, &map_encoder);

    if (ret != 0) {
        printf("Failed to encode data: %s \n", cbor_error_string(ret));
        return -1;
    }
    return 0;
}


size_t cbor_size_of(unsigned long v)
{
    int encoding_byte = 0;
    if (v > 0x18) {
        encoding_byte = 1;
    }

    if (v < 0xFF) {
        return encoding_byte + 1;
    }
    if (v < 0xFFFF) {
        return encoding_byte + 2;
    }
    if (v < 0xFFFFFFFF) {
        return encoding_byte + 4;
    }
    return 2;
}

void send_data(const gate_state_t state, const uint32_t timestamp)
{
    // (void)cbor_buf;
    // (void)buf_size;
    uint8_t identifier_sizes = 0;
    uint8_t counter_sizes = 0;
    uint8_t value_sizes = 0;
    for (int i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        identifier_sizes += cbor_size_of(ENCODE_SENSOR_TYPE_IDS(state.sensor_states[i].sensor_id,state.sensor_states[i].type, state.sensor_states[i].value_id));
        counter_sizes += cbor_size_of(state.sensor_states[i].event_counter);
        value_sizes += cbor_size_of(state.sensor_states[i].value);
    }

    cbor_buf_size = COMPUTE_CBOR_BUFFER_SIZE(cbor_size_of(SENSOR_CONFIG_LOCATION_POLDER), cbor_size_of(SENSOR_CONFIG_LOCATION_GATE), identifier_sizes, counter_sizes, value_sizes, cbor_size_of(seq_num), cbor_size_of(timestamp));
    printf("CBOR BUFF SIZE: %u\n", cbor_buf_size);
    cbor_buf = malloc((cbor_buf_size * sizeof(uint8_t)));

    encode_data(cbor_buf, cbor_buf_size, state, seq_num, timestamp);

    // send_lorawan_packet(cbor_buf, CBOR_BUF_SIZE);
    for (size_t i = 0; i < cbor_buf_size; i++) {
        printf("%02X", cbor_buf[i]);
    }
    printf("\n");

    seq_num++;

    free(cbor_buf);
}
