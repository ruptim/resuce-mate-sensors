#pragma once

#include "msg.h"

#include "dwax509m183x0.h"
#include "reed_sensor_driver.h"

#define SENSOR_TYPE_ID_DWAX509M183X0  1
#define SENSOR_TYPE_ID_REED_SWITCH 2
#define SENSOR_TYPE_ID_REED_SWITCH_NC 3
#define SENSOR_TYPE_ID_REED_SWITCH_NO 4

#define REED_SENSOR_DEBOUNCE_MS       60
#define REED_SENSOR_PIN_STATE_OPEN    0
#define REED_SENSOR_PIN_STATE_CLOSED  1
#define REED_SENSOR_NOT_ACTIVATED     0
#define REED_SENSOR_ACTIVATED         1

typedef enum {
    EQUAL_PARALLEL = 0b000,    /* equal priority, order doesn't matter */
    EQUAL_ORDERED = 0b001,     /* equal priority, order matters */
    WEIGHTED_PARALLEL = 0b010, /* different weights/priorities, order doesn't matter */
    WEIGHTED_ORDERED = 0b011,  /* different weights/priorities, order matters */
    MAJORITY_PARALLEL = 0b100, /* different weights/priorities, order doesn't matter */
    MAJORITY_ORDERED = 0b101,  /* different weights/priorities, order matters */

} multi_sensor_mode_t;


#define SENSOR_ENCODE_TYPE_BITS 4
#define SENSOR_ENCODE_SENSOR_ID_BITS 3
#define SENSOR_ENCODE_VALUE_ID_BITS 4
#define SENSOR_ENCODE_SENSOR_TYPE_MASK ((0x1 << SENSOR_ENCODE_TYPE_BITS)-1)
#define SENSOR_ENCODE_VALUE_ID_MASK ((0x1 << SENSOR_ENCODE_VALUE_ID_BITS)-1)
#define ENCODE_SENSOR_TYPE_IDS_BITS (SENSOR_ENCODE_TYPE_BITS+SENSOR_ENCODE_SENSOR_ID_BITS+SENSOR_ENCODE_VALUE_ID_BITS)
/*
* Macro to encode sensor type and global sensor id in a integer whose size 
* is defined by SENSOR_ENCODE_TYPE_BITS and SENSOR_ENCODE_ID_BITS.
* This allows for up to SENSOR_ENCODE_ID_BITS^2  different sensor IDs and
* SENSOR_ENCODE_TYPE_BITS different types.
* Sizes can be adjusted to fit requirements
* -----------------------------------------------------------
* |   Sensor Type (X bits)     |   Sensor Number (X bits)   |
* |---------------------------------------------------------|
*/
#define ENCODE_SENSOR_TYPE_IDS(sensor_id, type, value_id) (( (sensor_id) << SENSOR_ENCODE_TYPE_BITS | (type) ) << SENSOR_ENCODE_VALUE_ID_BITS | (value_id))


#define DECODE_SENSOR_ID(type_id_value) (type_id_value >> (SENSOR_ENCODE_TYPE_BITS +SENSOR_ENCODE_VALUE_ID_BITS))
#define DECODE_SENSOR_TYPE(type_id_value) ((type_id_value >> SENSOR_ENCODE_VALUE_ID_BITS) & SENSOR_ENCODE_SENSOR_TYPE_MASK)
#define DECODE_VALUE_ID(type_id_value) (type_id_value & SENSOR_ENCODE_VALUE_ID_MASK)



// Select uint type based on SENSOR_ENCODE_TYPE_BITS
#if SENSOR_ENCODE_TYPE_BITS <= 8
    typedef uint8_t sensor_type_t;
#elif SENSOR_ENCODE_TYPE_BITS <= 16
    typedef uint16_t sensor_type_t;
#elif SENSOR_ENCODE_TYPE_BITS <= 32
   typedef uint32_t sensor_type_t;
#elif SENSOR_ENCODE_TYPE_BITS <= 64
    #define SELECTED_UINT_TYPE uint64_t
    typedef uint64_t sensor_type_t;
#else
    #error "SENSOR_ENCODE_TYPE_BITS exceeds supported maximum."
#endif

// Select uint type based on SENSOR_ENCODE_ID_BITS
#if SENSOR_ENCODE_ID_BITS <= 8
    typedef uint8_t sensor_id_t;
#elif SENSOR_ENCODE_ID_BITS <= 16
    typedef uint16_t sensor_id_t;
#elif SENSOR_ENCODE_ID_BITS <= 32
   typedef uint32_t sensor_id_t;
#elif SENSOR_ENCODE_ID_BITS <= 64
    #define SELECTED_UINT_TYPE uint64_t
    typedef uint64_t sensor_id_t;
#else
    #error "SENSOR_ENCODE_ID_BITS exceeds supported maximum."
#endif



typedef union {
    reed_sensor_driver_t reed_sensor;
    dwax509m183x0_t inductive_sensor;
} sensor_base_type_t;

typedef union {
    reed_sensor_driver_params_t reed_sensor_params;
    dwax509m183x0_params_t inductive_sensor_params;
} sensor_base_params_t;

typedef struct {
    kernel_pid_t pid; // thread that receives the msg
    msg_t msg;        // preallocated scratchspace for msg;
} alarm_cb_args_t;

void dwax_alarm_cb(void *arg);

/**
 * @brief Callback function the normally-closed pin of the reed switch triggering the readout of the NC pin.
 *
 * @param args the void* of the alarm_cb_args_t struct.
 */
void reed_nc_callback(void *args);

/**
 * @brief Callback function the normally-open pin of the reed switch triggering the readout of the NO pin.
 *
 * @param args the void* of the alarm_cb_args_t struct.
 */
void reed_no_callback(void *args);

/**
 * @brief Callback function for the normally-closed pin of the reed switch, which also triggers the readout of the
 *        dwax... sensor.
 *
 * @param args the complete alarm_cb_args_t array with the dwax config at [0] and the nc config at [1].
 */
void reed_nc_callback_and_dwax_trigger(void *args);
