#pragma once

#include "stdio.h"


/** 
* @brief CBOR message structure:
* --------------------------------------------
* {
*     "l1": SENSOR_CONFIG_LOCATION_POLDER,
*     "l2": SENSOR_CONFIG_LOCATION_GATE,
*     "tb": SENSOR_ENCODE_TYPE_BITS,
*     "ib": SENSOR_ENCODE_SENSOR_ID_BITS,
*     "vb": SENSOR_ENCODE_VALUE_ID_BITS,
*     "i": [
*         ENCODE_SENSOR_TYPE_ID(type_1,id_1),
*         ...,
*         ENCODE_SENSOR_TYPE_ID(type_n,id_n),
*     ],
*     "v": [
*         <sensor_value_1,>
*         ...,
*         <sensor_value_n>,
* 
*     ],
*     "c": [
*         <sensor_event_counter_1>,
*         ...,
*         <sensor_event_counter_n>,
*     ],
*     "g": <current gate state>,
*     "s": <sequence number,
*     "t": <timestamp of gate state>
* }
* --------------------------------------------
*/

#define CBOR_MSG_MAP_LENGTH                     11 // items in map

/* bytes needed to encode data types. includes map, text (labels), arrays */
#define CBOR_MSG_TYPE_BYTES                     (CBOR_MSG_MAP_LENGTH + 1 + 3) // Map items + map + arrays
#define CBOR_MSG_TYPE_ENCODING_BYTES            3                             // seq. num, timestamp,
#define CBOR_MSG_LOCATION_PART_STRING_SIZE      2
#define CBOR_MSG_LOCATION_PART_VALUE_SIZE       1 // 8 bit
#define CBOR_MSG_SENSOR_ENCODE_BITS_STRING_SIZE 2
#define CBOR_MSG_SENSOR_ENCODE_BITS_VALUE_SIZE  1 // 8 bit
#define CBOR_MSG_SENSOR_IDENTIFIER_STRING_SIZE  1
#define CBOR_MSG_SENSOR_IDENTIFIER_VALUE_SIZE   (ENCODE_SENSOR_TYPE_IDS_BITS) // defined in "sensors.h"
#define CBOR_MSG_SENSOR_VALUE_STRING_SIZE       1
#define CBOR_MSG_SENSOR_VALUES_SIZE             (SENSORS_MAX_VALUE_BYTES_NEEDED) // defined in "sensor_config.h"
#define CBOR_MSG_SENSOR_COUNTER_STRING_SIZE     1
#define CBOR_MSG_SENSOR_COUNTER_VALUE_SIZE      1 // 8 bit
#define CBOR_MSG_SENSOR_GATE_STATE_STRING_SIZE  1
#define CBOR_MSG_SENSOR_GATE_STATE_VALUE_SIZE   1 // bool
#define CBOR_MSG_SENSOR_SEQ_NUM_STRING_SIZE     1
#define CBOR_MSG_SENSOR_SEQ_NUM_VALUE_SIZE      4 // 32-bit
#define CBOR_MSG_SENSOR_TIMESTAMP_STRING_SIZE   1
#define CBOR_MSG_SENSOR_TIMESTAMP_VALUE_SIZE    4 // 32-bit

#define CBOR_BUFFER_SIZE                        CBOR_MSG_TYPE_BYTES +        \
                             (CBOR_MSG_LOCATION_PART_STRING_SIZE * 2) +      \
                             (CBOR_MSG_LOCATION_PART_VALUE_SIZE * 2) +       \
                             (CBOR_MSG_SENSOR_ENCODE_BITS_STRING_SIZE * 3) + \
                             (CBOR_MSG_SENSOR_ENCODE_BITS_VALUE_SIZE * 3) +  \
                             CBOR_MSG_SENSOR_IDENTIFIER_STRING_SIZE +        \
                             CBOR_MSG_SENSOR_COUNTER_STRING_SIZE +           \
                             ((CBOR_MSG_SENSOR_IDENTIFIER_VALUE_SIZE +       \
                               CBOR_MSG_SENSOR_COUNTER_VALUE_SIZE) *         \
                              NUM_UNIQUE_SENSOR_VALUES) +                    \
                             CBOR_MSG_SENSOR_VALUE_STRING_SIZE +             \
                             CBOR_MSG_SENSOR_VALUES_SIZE +                   \
                             CBOR_MSG_SENSOR_GATE_STATE_STRING_SIZE +        \
                             CBOR_MSG_SENSOR_GATE_STATE_VALUE_SIZE +         \
                             CBOR_MSG_SENSOR_SEQ_NUM_STRING_SIZE +           \
                             CBOR_MSG_SENSOR_SEQ_NUM_VALUE_SIZE +            \
                             CBOR_MSG_SENSOR_TIMESTAMP_STRING_SIZE +         \
                             CBOR_MSG_SENSOR_TIMESTAMP_VALUE_SIZE

#define COMPUTE_CBOR_BUFFER_SIZE(polder_num_size, gate_num_size, identifier_value_sizes, counter_sizes, value_sizes, seq_num_size, timestamp_size) CBOR_MSG_TYPE_BYTES +                                        \
                                                                                                                                                       (CBOR_MSG_LOCATION_PART_STRING_SIZE + polder_num_size) + \
                                                                                                                                                       (CBOR_MSG_LOCATION_PART_STRING_SIZE +                    \
                                                                                                                                                        gate_num_size) +                                        \
                                                                                                                                                       (CBOR_MSG_SENSOR_ENCODE_BITS_STRING_SIZE +               \
                                                                                                                                                        CBOR_MSG_SENSOR_ENCODE_BITS_VALUE_SIZE) +               \
                                                                                                                                                       (CBOR_MSG_SENSOR_ENCODE_BITS_STRING_SIZE +               \
                                                                                                                                                        CBOR_MSG_SENSOR_ENCODE_BITS_VALUE_SIZE) +               \
                                                                                                                                                       (CBOR_MSG_SENSOR_ENCODE_BITS_STRING_SIZE +               \
                                                                                                                                                        CBOR_MSG_SENSOR_ENCODE_BITS_VALUE_SIZE) +               \
                                                                                                                                                       CBOR_MSG_SENSOR_IDENTIFIER_STRING_SIZE +                 \
                                                                                                                                                       identifier_value_sizes +                                 \
                                                                                                                                                       CBOR_MSG_SENSOR_COUNTER_STRING_SIZE +                    \
                                                                                                                                                       counter_sizes +                                          \
                                                                                                                                                       CBOR_MSG_SENSOR_VALUE_STRING_SIZE +                      \
                                                                                                                                                       value_sizes +                                            \
                                                                                                                                                       CBOR_MSG_SENSOR_GATE_STATE_STRING_SIZE +                 \
                                                                                                                                                       CBOR_MSG_SENSOR_GATE_STATE_VALUE_SIZE +                  \
                                                                                                                                                       CBOR_MSG_SENSOR_SEQ_NUM_STRING_SIZE +                    \
                                                                                                                                                       seq_num_size +                                           \
                                                                                                                                                       CBOR_MSG_SENSOR_TIMESTAMP_STRING_SIZE +                  \
                                                                                                                                                       timestamp_size

                                                                                                                                                       
