/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Interaction between the application layer and the two types of drivers being evaluated for the rescue mate project
 *
 * @author      Jan Thies <jan.thies@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "msg.h"
#include "thread.h"
#include "periph/rtc.h"

#include "dwax509m183x0.h"
#include "dwax509m183x0_params.h"

#include "reed_sensor_driver.h"
#include "reed_sensor_driver_params.h"



#define RCV_QUEUE_SIZE 4
static msg_t rcv_queue[RCV_QUEUE_SIZE];

#define NONE 0
#define SENSOR_DWAX509M183X0 1
#define SENSOR_REED_SWITCH_NC 2
#define SENSOR_REED_SWITCH_NO 3

#define REED_SENSOR_DEBOUNCE_MS 20
gpio_t nc_pin = GPIO_PIN(0, 8);
gpio_t no_pin = GPIO_PIN(0, 6);




typedef struct {
    kernel_pid_t pid;   // thread that receives the msg
    msg_t msg;          // preallocated scratchspace for msg;
} alarm_cb_args_t;

void dwax_alarm_cb(void *arg)
{
    alarm_cb_args_t *cb_args = (alarm_cb_args_t *)arg;

    msg_send_int(&cb_args->msg, cb_args->pid);
}

void get_future_time_s(struct tm *time, uint32_t seconds)
{
    rtc_get_time(time);

    time_t now_epoch = mktime(time);
    time_t alarm_epoch = now_epoch + seconds;

    gmtime_r(&alarm_epoch, time);
}

void reed_nc_callback(void *args)
{
    (void)args;


    msg_t msg = {
        .type = (SENSOR_REED_SWITCH_NC << 8) | (1),
        .content.ptr = (void *)&sensor_02
    };

    msg_send_int(&msg, (kernel_pid_t)args);
}

void reed_no_callback(void *args)
{
    (void)args;

    msg_t msg = {
        .type = (SENSOR_REED_SWITCH_NO << 8) | (0),
        .content.ptr = (void *)&sensor_02
    };

    msg_send_int(&msg, (kernel_pid_t)args);

}


static dwax509m183x0_t sensor_01;
static reed_sensor_driver_t sensor_02;

#define NUM_SENSORS 1
static alarm_cb_args_t alarm_cb_args[NUM_SENSORS];

int main(void)
{
    puts("Generated RIOT application: 'rescue_mate'");
    msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);
    memset(alarm_cb_args, 0, sizeof(alarm_cb_args));

    // Initialize all connected sensors
    dwax509m183x0_init(&sensor_01, &dwax509m183x0_params[0]);


    reed_sensor_driver_params_t params = { .nc_pin = nc_pin,
                                           .no_pin = no_pin,
                                           .nc_int_flank = GPIO_BOTH,
                                           .no_int_flank = GPIO_BOTH,
                                           .nc_callback = reed_nc_callback,
                                           .no_callback = reed_no_callback,
                                           .nc_callback_args = (void *)thread_getpid(),
                                           .no_callback_args = (void *)thread_getpid(),
                                           .use_external_pulldown = false,
                                           .debounce_ms = REED_SENSOR_DEBOUNCE_MS };

    reed_sensor_driver_init(&sensor_02, &params);



    // Set up alarms / irqs for all the sensors (i.e. decide whether they shall be called via timer isr, gpio isr etc)
    alarm_cb_args[0].pid = thread_getpid();
    alarm_cb_args[0].msg.type = (SENSOR_DWAX509M183X0 << 8) | (0);
    alarm_cb_args[0].msg.content.ptr = (void *)&sensor_01;

    // Get a timestamp in one hour
    rtc_init();

    struct tm time = (struct tm){ 0 };
    get_future_time_s(&time, 3600);
    rtc_set_alarm(&time, &dwax_alarm_cb, &alarm_cb_args[0]);


    msg_t msg;
    while (true) {
        msg_receive(&msg);
        uint8_t const sensor_type = msg.type >> 8;
        uint8_t const sensor_id = msg.type & 0x00FF;

        switch (sensor_type) {
        case SENSOR_DWAX509M183X0:

            dwax509m183x0_t *dev = (dwax509m183x0_t *)msg.content.ptr;
            int distance_um = dwax509m183x0_distance_um(dev);
            get_future_time_s(&time, 3600);
            rtc_set_alarm(&time, &dwax_alarm_cb, &alarm_cb_args[sensor_id]);

            break;
        case SENSOR_REED_SWITCH_NC:
            reed_sensor_driver_t *dev = (reed_sensor_driver_t *)msg.content.ptr;
            reed_sensor_val_t nc_val;
            reed_sensor_driver_read_nc(dev, &nc_val);

            break;
        case SENSOR_REED_SWITCH_NO:
            reed_sensor_driver_t *dev = (reed_sensor_driver_t *)msg.content.ptr;
            reed_sensor_val_t no_val;
            reed_sensor_driver_read_no(dev, &no_val);

            break;
        }
    }


    return 0;
}
