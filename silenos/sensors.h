#ifndef SENSORS_H_
#define SENSORS_H_

#include "msg.h"
#include "thread.h"

typedef struct
{
    kernel_pid_t pid; // thread that receives the msg
    msg_t msg;        // preallocated scratchspace for msg;
} alarm_cb_args_t;

void dwax_alarm_cb(void *arg)
{
    alarm_cb_args_t *cb_args = (alarm_cb_args_t *)arg;

    msg_send_int(&cb_args->msg, cb_args->pid);
}

/**
 * @brief Callback function the normally-closed pin of the reed switch triggering the readout of the NC pin.
 *
 * @param args the void* of the alarm_cb_args_t struct.
 */
void reed_nc_callback(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;

    msg_send_int(&msg_args->msg, msg_args->pid);
}

/**
 * @brief Callback function the normally-open pin of the reed switch triggering the readout of the NO pin.
 *
 * @param args the void* of the alarm_cb_args_t struct.
 */
void reed_no_callback(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;

    msg_send_int(&msg_args->msg, msg_args->pid);
}

/**
 * @brief Callback function for the normally-closed pin of the reed switch, which also triggers the readout of the
 *        dwax... sensor.
 *
 * @param args the complete alarm_cb_args_t array with the dwax config at [0] and the nc config at [1].
 */
void reed_nc_callback_and_dwax_trigger(void *args)
{
    (void)args;
    alarm_cb_args_t *msg_args = (alarm_cb_args_t *)args;

    msg_send_int(&msg_args[1].msg, msg_args[1].pid);
    msg_send_int(&msg_args[0].msg, msg_args[0].pid);
}


#endif // SENSORS_H_