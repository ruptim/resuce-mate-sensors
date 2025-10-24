#include "sensors.h"

void dwax_alarm_cb(void *arg)
{
    alarm_cb_args_t *cb_args = (alarm_cb_args_t *)arg;

    msg_send_int(&cb_args->msg, cb_args->pid);
}


void reed_nc_callback(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;

    // uint8_t const sensor_type = msg_args->msg.type >> 8;
    // uint8_t const sensor_id = msg_args->msg.type & 0x00FF;
    // printf("NC: %d, %d\n", sensor_type, sensor_id);
    msg_send_int(&msg_args->msg, msg_args->pid);
}


void reed_no_callback(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;
    // uint8_t const sensor_type = msg_args->msg.type >> 8;
    // uint8_t const sensor_id = msg_args->msg.type & 0x00FF;
    // printf("NO: %d, %d\n", sensor_type, sensor_id);
    msg_send_int(&msg_args->msg, msg_args->pid);
}


void reed_nc_callback_and_dwax_trigger(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;

    msg_send_int(&msg_args[1].msg, msg_args[1].pid);
    msg_send_int(&msg_args[0].msg, msg_args[0].pid);
}
