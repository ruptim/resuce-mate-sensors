#include "messages.h"
#include "cbor_encoding.h"
#include "lora_networking.h"
#include "cbor.h"

#include "sensors.h"
#include "state_validation.h"
#include "stdio.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t cbor_buf[CBOR_BUFFER_SIZE] = { 0 };
static uint16_t cbor_buf_size = CBOR_BUFFER_SIZE;
// static uint8_t *cbor_buf;

static int seq_num = 0;


/**
 * @brief Calculate bytes needed for encoding a value in CBOR. RFC 8949.
 * 
 * @param buf           cbor buffer for the encoded data
 * @param buf_size      len of the cbor buffer
 * @param gate_state    gate state with the data about sensors and gate to encode
 * @param seq_num       current sequence number
 * @param seq_num       local timestamp of time of sending 
 * @return              -1 when failing to encode the data.
 */
static int encode_data(uint8_t *buf, size_t buf_size, gate_state_t gate_state, int seq_num, uint32_t timestamp_s);


/**
 * @brief Calculate required size to encode a given unsigned integer with CBOR.  
 * 
 * @param v        Value to get the required size for.
 * @return size_t  The required size.
 */
static size_t cbor_size_of(unsigned long v);



static int encode_data(uint8_t *buf,
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
        sensor_value_state_t sensor = gate_state.sensor_value_states[i];
        ret |= cbor_encode_uint(&array_encoder_i, ENCODE_SENSOR_TYPE_IDS(sensor.sensor_id,sensor.type, sensor.value_id));
    }
    ret |= cbor_encoder_close_container(&map_encoder, &array_encoder_i);

    /* create key 'v' value array */
    ret |= cbor_encode_text_stringz(&map_encoder, "v"); /* 1 byte */
    ret |= cbor_encoder_create_array(&map_encoder, &array_encoder_v, NUM_UNIQUE_SENSOR_VALUES);
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; ++i) {
        sensor_value_state_t sensor = gate_state.sensor_value_states[i];

        ret |= cbor_encode_uint(&array_encoder_v, sensor.value);
    }
    ret |= cbor_encoder_close_container(&map_encoder, &array_encoder_v);

    /* create key 'c' event counter array */
    ret |= cbor_encode_text_stringz(&map_encoder, "c"); /* 1 byte */
    ret |= cbor_encoder_create_array(&map_encoder, &array_encoder_c, NUM_UNIQUE_SENSOR_VALUES);

    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; ++i) {
        sensor_value_state_t sensor = gate_state.sensor_value_states[i];

        ret |= cbor_encode_uint(&array_encoder_c, sensor.event_counter);
    }
    ret |= cbor_encoder_close_container(&map_encoder, &array_encoder_c);

    /* create key 'k' ticket array */
    ret |= cbor_encode_text_stringz(&map_encoder, "k"); /* 1 byte */
    ret |= cbor_encoder_create_array(&map_encoder, &array_encoder_c, NUM_UNIQUE_SENSOR_VALUES);

    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; ++i) {
        sensor_value_state_t sensor = gate_state.sensor_value_states[i];

        ret |= cbor_encode_uint(&array_encoder_c, sensor.latest_arrive_ticket);
    }
    ret |= cbor_encoder_close_container(&map_encoder, &array_encoder_c);

    /* create key 'g' gate state */
    ret |= cbor_encode_text_stringz(&map_encoder, "g"); /* 1 byte */
    ret |= cbor_encode_boolean(&map_encoder, gate_state.gate_closed);

    /* create key 's' sequence number */
    ret |= cbor_encode_text_stringz(&map_encoder, "s"); /* 1 byte */
    ret |= cbor_encode_uint(&map_encoder, seq_num);

    /* create key 't' timestamp number */
    ret |= cbor_encode_text_stringz(&map_encoder, "t"); /* 1 byte */
    ret |= cbor_encode_uint(&map_encoder, timestamp_s);

    ret |= cbor_encoder_close_container(&encoder, &map_encoder);

    

    if (ret != 0) {
        printf("Failed to encode data: %s, Additional bytes needed: %d \n", cbor_error_string(ret),cbor_encoder_get_extra_bytes_needed(&encoder));
        return -1;
    }
    return 0;
}


static size_t cbor_size_of(unsigned long v)
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
    uint8_t identifier_sizes = 0;
    uint8_t counter_sizes = 0;
    uint8_t value_sizes = 0;
    uint8_t ticket_sizes = 0;


    /* compute sizes needed for types, event counters and values */
    for (int i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        identifier_sizes += cbor_size_of(ENCODE_SENSOR_TYPE_IDS(state.sensor_value_states[i].sensor_id,state.sensor_value_states[i].type, state.sensor_value_states[i].value_id));
        counter_sizes += cbor_size_of(state.sensor_value_states[i].event_counter);
        value_sizes += cbor_size_of(state.sensor_value_states[i].value);
        ticket_sizes += cbor_size_of(state.sensor_value_states[i].value);
    }

    /* compute required cbor buffer size and allocate it. */
    size_t cbor_buf_size_needed = COMPUTE_CBOR_BUFFER_SIZE(cbor_size_of(SENSOR_CONFIG_LOCATION_POLDER), cbor_size_of(SENSOR_CONFIG_LOCATION_GATE), identifier_sizes, counter_sizes, value_sizes,ticket_sizes, cbor_size_of(seq_num), cbor_size_of(timestamp));
    // cbor_buf =  malloc((cbor_buf_size * sizeof(uint8_t)));

    if (cbor_buf_size_needed > cbor_buf_size){
        //todo: error / allocate new one?
    }

    

    encode_data(cbor_buf, cbor_buf_size, state, seq_num, timestamp);

    
    send_lorawan_packet(cbor_buf, cbor_buf_size);


    for (size_t i = 0; i < cbor_buf_size; i++) {
        printf("%02X", cbor_buf[i]);
    }
    printf("\n");

    seq_num++;

    // free(cbor_buf);
}
