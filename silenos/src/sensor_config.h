#ifndef SENSOR_CONFIG_H_
#define SENSOR_CONFIG_H_

#include "sensors.h"
#include "dwax509m183x0.h"
#include "reed_sensor_driver.h"

#define SENSOR_TYPE_ID_DWAX509M183X0    1
#define SENSOR_TYPE_ID_REED_SWITCH_NC   2
#define SENSOR_TYPE_ID_REED_SWITCH_NO   3

#define REED_SENSOR_DEBOUNCE_MS         60

/*
* Macro to encode sensor type and global sensor id in a 16-Bit integer.
* ---------------------------------
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

/* ------------------------------Sensor Declaration------------------------------ */

/* Number of physical sensors connected (some sensors have multiple contacts e.g. reed sensors) */
#define NUM_SENSORS              2

#define NUM_UNIQUE_SENSOR_VALUES 4
static alarm_cb_args_t alarm_cb_args[NUM_UNIQUE_SENSOR_VALUES];

#define SENSOR_1_REED_NC_ID 0
#define SENSOR_1_REED_NO_ID 1

#define SENSOR_2_REED_NC_ID 2
#define SENSOR_2_REED_NO_ID 3

sensor_base_type_t registered_sensors[2];
sensor_base_params_t registered_sensors_params[2];
int init_sensors(void)
{
    alarm_cb_args[SENSOR_1_REED_NC_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_1_REED_NC_ID].msg.type =
        ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC, 0);
    alarm_cb_args[SENSOR_1_REED_NC_ID].msg.content.ptr = (void *)&registered_sensors[0];

    alarm_cb_args[SENSOR_1_REED_NO_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_1_REED_NO_ID].msg.type =
        ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO, 1);
    alarm_cb_args[SENSOR_1_REED_NO_ID].msg.content.ptr = (void *)&registered_sensors[0];

    //                                first cast to specific param type and then to base params type for the array.
    registered_sensors_params[0] = (sensor_base_params_t)(reed_sensor_driver_params_t){
        .nc_pin = GPIO_PIN(1, 9),
        .no_pin = GPIO_PIN(0, 8),
        .nc_int_flank = GPIO_BOTH,
        .no_int_flank = GPIO_BOTH,
        .nc_callback = reed_nc_callback,
        .no_callback = reed_no_callback,
        .nc_callback_args = (void *)&alarm_cb_args[SENSOR_1_REED_NC_ID],
        .no_callback_args = (void *)&alarm_cb_args[SENSOR_1_REED_NO_ID],
        .use_external_pulldown = false,
        .debounce_ms = REED_SENSOR_DEBOUNCE_MS
    };
    reed_sensor_driver_init(&registered_sensors[0].reed_sensor,
                            &registered_sensors_params[0].reed_sensor_params);

    alarm_cb_args[SENSOR_2_REED_NC_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_2_REED_NC_ID].msg.type =
        ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC, 2);
    alarm_cb_args[SENSOR_2_REED_NC_ID].msg.content.ptr = (void *)&registered_sensors[1];

    alarm_cb_args[SENSOR_2_REED_NO_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_2_REED_NO_ID].msg.type =
        ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO, 3);
    alarm_cb_args[SENSOR_2_REED_NO_ID].msg.content.ptr = (void *)&registered_sensors[1];

    //                                first cast to specific param type and then to base params type for the array.
    registered_sensors_params[1] = (sensor_base_params_t)(reed_sensor_driver_params_t){
        .nc_pin = GPIO_PIN(0, 6),
        .no_pin = GPIO_PIN(0, 27),
        .nc_int_flank = GPIO_BOTH,
        .no_int_flank = GPIO_BOTH,
        .nc_callback = reed_nc_callback,
        .no_callback = reed_no_callback,
        .nc_callback_args = (void *)&alarm_cb_args[SENSOR_2_REED_NC_ID],
        .no_callback_args = (void *)&alarm_cb_args[SENSOR_2_REED_NO_ID],
        .use_external_pulldown = false,
        .debounce_ms = REED_SENSOR_DEBOUNCE_MS
    };
    reed_sensor_driver_init(&registered_sensors[1].reed_sensor,
                            &registered_sensors_params[1].reed_sensor_params);

    return 0;
}
#endif // SENSOR_CONFIG_H_
