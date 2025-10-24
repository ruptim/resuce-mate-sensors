#include "sensor_config.h"

sensor_base_type_t registered_sensors[3];
sensor_base_params_t registered_sensors_params[3];

int init_sensors(void)
{
    int ret = 0;
    /* -------------------- init code for sensor 'sensor_1_reed' -------------------- */
    alarm_cb_args[SENSOR_1_REED_NC_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_1_REED_NC_ID].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC,0);
    alarm_cb_args[SENSOR_1_REED_NC_ID].msg.content.ptr = (void *)&registered_sensors[0];
    
    alarm_cb_args[SENSOR_1_REED_NO_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_1_REED_NO_ID].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO,1);
    alarm_cb_args[SENSOR_1_REED_NO_ID].msg.content.ptr = (void *)&registered_sensors[0];
    
    /* first cast to specific param type and then to base params type for the array. */
    registered_sensors_params[0] = (sensor_base_params_t) (reed_sensor_driver_params_t) {
        .nc_pin = GPIO_PIN(1,9),
        .no_pin = GPIO_PIN(0,8),
        .nc_int_flank = GPIO_BOTH,
        .no_int_flank = GPIO_BOTH,
        .nc_callback = reed_nc_callback,
        .no_callback = reed_no_callback,
        .nc_callback_args = (void *)&alarm_cb_args[SENSOR_1_REED_NC_ID],
        .no_callback_args = (void *)&alarm_cb_args[SENSOR_1_REED_NO_ID],
        .use_external_pulldown = false,
        .debounce_ms = REED_SENSOR_DEBOUNCE_MS };
    
    if ((ret = reed_sensor_driver_init(&registered_sensors[0].reed_sensor, &registered_sensors_params[0].reed_sensor_params)) != 0){
    	return ret;
    }
    
    
    /* -------------------- init code for sensor 'sensor_2_reed' -------------------- */
    alarm_cb_args[SENSOR_2_REED_NC_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_2_REED_NC_ID].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC,2);
    alarm_cb_args[SENSOR_2_REED_NC_ID].msg.content.ptr = (void *)&registered_sensors[1];
    
    alarm_cb_args[SENSOR_2_REED_NO_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_2_REED_NO_ID].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO,3);
    alarm_cb_args[SENSOR_2_REED_NO_ID].msg.content.ptr = (void *)&registered_sensors[1];
    
    /* first cast to specific param type and then to base params type for the array. */
    registered_sensors_params[1] = (sensor_base_params_t) (reed_sensor_driver_params_t) {
        .nc_pin = GPIO_PIN(0,6),
        .no_pin = GPIO_PIN(0,27),
        .nc_int_flank = GPIO_BOTH,
        .no_int_flank = GPIO_BOTH,
        .nc_callback = reed_nc_callback,
        .no_callback = reed_no_callback,
        .nc_callback_args = (void *)&alarm_cb_args[SENSOR_2_REED_NC_ID],
        .no_callback_args = (void *)&alarm_cb_args[SENSOR_2_REED_NO_ID],
        .use_external_pulldown = false,
        .debounce_ms = REED_SENSOR_DEBOUNCE_MS };
    
    if ((ret = reed_sensor_driver_init(&registered_sensors[1].reed_sensor, &registered_sensors_params[1].reed_sensor_params)) != 0){
    	return ret;
    }
    
    
    /* -------------------- init code for sensor 'sensor_3_reed' -------------------- */
    alarm_cb_args[SENSOR_3_REED_NC_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_3_REED_NC_ID].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC,4);
    alarm_cb_args[SENSOR_3_REED_NC_ID].msg.content.ptr = (void *)&registered_sensors[2];
    
    alarm_cb_args[SENSOR_3_REED_NO_ID].pid = thread_getpid();
    alarm_cb_args[SENSOR_3_REED_NO_ID].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO,5);
    alarm_cb_args[SENSOR_3_REED_NO_ID].msg.content.ptr = (void *)&registered_sensors[2];
    
    /* first cast to specific param type and then to base params type for the array. */
    registered_sensors_params[2] = (sensor_base_params_t) (reed_sensor_driver_params_t) {
        .nc_pin = GPIO_PIN(0,26),
        .no_pin = GPIO_PIN(0,7),
        .nc_int_flank = GPIO_BOTH,
        .no_int_flank = GPIO_BOTH,
        .nc_callback = reed_nc_callback,
        .no_callback = reed_no_callback,
        .nc_callback_args = (void *)&alarm_cb_args[SENSOR_3_REED_NC_ID],
        .no_callback_args = (void *)&alarm_cb_args[SENSOR_3_REED_NO_ID],
        .use_external_pulldown = false,
        .debounce_ms = REED_SENSOR_DEBOUNCE_MS };
    
    if ((ret = reed_sensor_driver_init(&registered_sensors[2].reed_sensor, &registered_sensors_params[2].reed_sensor_params)) != 0){
    	return ret;
    }
    
    
    return 0;
}