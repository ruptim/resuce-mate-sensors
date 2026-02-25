#include "sensors.h"

void dwas_alarm_cb(void *arg)
{
    alarm_cb_args_t *cb_args = (alarm_cb_args_t *)arg;

    msg_send_int(&cb_args->msg, cb_args->pid);
}

void reed_nc_callback(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;

    msg_send_int(&msg_args->msg, msg_args->pid);
}

void reed_no_callback(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;
    msg_send_int(&msg_args->msg, msg_args->pid);
}

void reed_nc_callback_and_dwax_trigger(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;

    msg_send_int(&msg_args[1].msg, msg_args[1].pid);
    msg_send_int(&msg_args[0].msg, msg_args[0].pid);
}
