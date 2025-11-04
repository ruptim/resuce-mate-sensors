#pragma once

#include "msg.h"

#include "dwax509m183x0.h"
#include "reed_sensor_driver.h"

#define SENSOR_TYPE_ID_DWAX509M183X0  1
#define SENSOR_TYPE_ID_REED_SWITCH_NC 2
#define SENSOR_TYPE_ID_REED_SWITCH_NO 3

#define REED_SENSOR_DEBOUNCE_MS       60
#define REED_SENSOR_PIN_STATE_OPEN    0
#define REED_SENSOR_PIN_STATE_CLOSED  1
#define REED_SENSOR_NOT_ACTIVATED     0
#define REED_SENSOR_ACTIVATED         1

typedef enum {
    EQUAL_PARALLEL = 0b00,    /* equal priority, order doesn't matter */
    EQUAL_ORDERED = 0b01,     /* equal priority, order matters */
    WEIGHTED_PARALLEL = 0b10, /* different weights/priorities, order doesn't matter */
    WEIGHTED_ORDERED = 0b11,  /* different weights/priorities, order matters */
} multi_sensor_mode_t;

/*
* Macro to encode sensor type and global sensor id in a 16-Bit integer.
* -----------------------------------------------------------
* |   Sensor Type (8 bits)     |   Sensor Number (8 bits)   |
* |----------------------------|----------------------------|
* |  7  6  5  4  3  2  1  0    |  7  6  5  4  3  2  1  0    |
* |---------------------------------------------------------|
*/
#define ENCODE_SENSOR_TYPE_ID(type, id) ((type) << 8 | (id))

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
