#include "string.h"
#include "sensor_config.h"

alarm_cb_args_t alarm_cb_args[NUM_UNIQUE_SENSOR_VALUES];
sensor_base_type_t registered_sensors[3];
sensor_base_params_t registered_sensors_params[3];

int init_sensors(kernel_pid_t receive_pid)
{
    memset(alarm_cb_args, 0, sizeof(alarm_cb_args));

    int ret = 0;
    /* -------------------- init code for sensor 'sensor_1_reed' -------------------- */
    alarm_cb_args[SENSOR_1_REED_NO_ID].pid = receive_pid;
    alarm_cb_args[SENSOR_1_REED_NO_ID].msg.type = ENCODE_SENSOR_TYPE_IDS(0,SENSOR_TYPE_ID_REED_SWITCH_NO,SENSOR_1_REED_NO_ID);
    alarm_cb_args[SENSOR_1_REED_NO_ID].msg.content.ptr = (void *)&registered_sensors[SENSOR_1_ID];
    
    /* first cast to specific param type and then to base params type for the array. */
    registered_sensors_params[SENSOR_1_ID] = (sensor_base_params_t) (reed_sensor_driver_params_t) {
        .no_pin = GPIO_PIN(0,8),
        .no_int_flank = GPIO_BOTH,
        .no_callback = reed_no_callback,
        .no_callback_args = (void *)&alarm_cb_args[SENSOR_1_REED_NO_ID],
        .use_external_pulldown = false,
        .debounce_ms = REED_SENSOR_DEBOUNCE_MS };
    
    if ((ret = reed_sensor_driver_init(&registered_sensors[SENSOR_1_ID].reed_sensor, &registered_sensors_params[SENSOR_1_ID].reed_sensor_params)) != 0){
    	return ret;
    }
    
    
    /* -------------------- init code for sensor 'sensor_2_reed' -------------------- */
    alarm_cb_args[SENSOR_2_REED_NO_ID].pid = receive_pid;
    alarm_cb_args[SENSOR_2_REED_NO_ID].msg.type = ENCODE_SENSOR_TYPE_IDS(1,SENSOR_TYPE_ID_REED_SWITCH_NO,SENSOR_2_REED_NO_ID);
    alarm_cb_args[SENSOR_2_REED_NO_ID].msg.content.ptr = (void *)&registered_sensors[SENSOR_2_ID];
    
    /* first cast to specific param type and then to base params type for the array. */
    registered_sensors_params[SENSOR_2_ID] = (sensor_base_params_t) (reed_sensor_driver_params_t) {
        .no_pin = GPIO_PIN(0,12),
        .no_int_flank = GPIO_BOTH,
        .no_callback = reed_no_callback,
        .no_callback_args = (void *)&alarm_cb_args[SENSOR_2_REED_NO_ID],
        .use_external_pulldown = false,
        .debounce_ms = REED_SENSOR_DEBOUNCE_MS };
    
    if ((ret = reed_sensor_driver_init(&registered_sensors[SENSOR_2_ID].reed_sensor, &registered_sensors_params[SENSOR_2_ID].reed_sensor_params)) != 0){
    	return ret;
    }
    
    
    /* -------------------- init code for sensor 'sensor_3_reed' -------------------- */
    alarm_cb_args[SENSOR_3_REED_NO_ID].pid = receive_pid;
    alarm_cb_args[SENSOR_3_REED_NO_ID].msg.type = ENCODE_SENSOR_TYPE_IDS(2,SENSOR_TYPE_ID_REED_SWITCH_NO,SENSOR_3_REED_NO_ID);
    alarm_cb_args[SENSOR_3_REED_NO_ID].msg.content.ptr = (void *)&registered_sensors[SENSOR_3_ID];
    
    /* first cast to specific param type and then to base params type for the array. */
    registered_sensors_params[SENSOR_3_ID] = (sensor_base_params_t) (reed_sensor_driver_params_t) {
        .no_pin = GPIO_PIN(0,5),
        .no_int_flank = GPIO_BOTH,
        .no_callback = reed_no_callback,
        .no_callback_args = (void *)&alarm_cb_args[SENSOR_3_REED_NO_ID],
        .use_external_pulldown = false,
        .debounce_ms = REED_SENSOR_DEBOUNCE_MS };
    
    if ((ret = reed_sensor_driver_init(&registered_sensors[SENSOR_3_ID].reed_sensor, &registered_sensors_params[SENSOR_3_ID].reed_sensor_params)) != 0){
    	return ret;
    }
    
    
    return 0;
}