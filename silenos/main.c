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
#include "ztimer.h"
 

#include "cbor.h"

#include "dwax509m183x0.h"
#include "dwax509m183x0_params.h"

#include "reed_sensor_driver.h"
#include "reed_sensor_driver_params.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define RCV_QUEUE_SIZE 4
static msg_t rcv_queue[RCV_QUEUE_SIZE];

#define NONE 0
#define SENSOR_DWAX509M183X0 1
#define SENSOR_REED_SWITCH_NC 2
#define SENSOR_REED_SWITCH_NO 3

#define REED_SENSOR_DEBOUNCE_MS 60
gpio_t nc_pin = GPIO_PIN(0,6); // D11
gpio_t no_pin = GPIO_PIN(1, 9); // D13


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

int encode_data(uint8_t *buf, size_t buf_size, int *data, int data_len, int event_counter, int seq_num)
{
    int ret = 0;

    CborEncoder encoder, arrayEncoder, mapEncoder;
    /* init and create map */
    cbor_encoder_init(&encoder, buf, buf_size, 0);
    ret |= cbor_encoder_create_map(&encoder, &mapEncoder, 3);

    /* create key 'd' data array */
    ret |= cbor_encode_text_stringz(&mapEncoder, "d");
    ret |= cbor_encoder_create_array(&mapEncoder, &arrayEncoder, data_len);
    for (int i = 0; i < data_len; ++i)
    {
        ret |= cbor_encode_int(&arrayEncoder, data[i]);
    }
    ret |= cbor_encoder_close_container(&mapEncoder, &arrayEncoder);

    /* create key 'c' for event counter */
    ret |= cbor_encode_text_stringz(&mapEncoder, "c");
    ret |= cbor_encode_int(&mapEncoder, event_counter);
    /* create key 's' sequence number */
    ret |= cbor_encode_text_stringz(&mapEncoder, "s");
    ret |= cbor_encode_int(&mapEncoder, seq_num);

    ret |= cbor_encoder_close_container(&encoder, &mapEncoder);

    if (ret != 0)
    {
        printf("Failed to encode data: %s \n", cbor_error_string(ret));
        return -1;
    }
    return 0;
}

int send_data(uint8_t *cbor_buf, size_t buf_size)
{
    // todo;
    (void)cbor_buf;
    (void)buf_size;

    return 0;
}


static dwax509m183x0_t sensor_01;
static reed_sensor_driver_t sensor_02;

#define NUM_SENSORS 3
static alarm_cb_args_t alarm_cb_args[NUM_SENSORS];

#define ALARM_TIMER_INTERVAL_S 60 * 60
ztimer_t alarm_timer;

#define CBOR_BUF_SIZE 40 

int main(void)
{
    puts("Generated RIOT application: 'rescue_mate'");
    msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);
    memset(alarm_cb_args, 0, sizeof(alarm_cb_args));

    // Initialize all connected sensors
    dwax509m183x0_init(&sensor_01, &dwax509m183x0_params[0]);

    alarm_cb_args[1].pid = thread_getpid();
    alarm_cb_args[1].msg.type = (SENSOR_REED_SWITCH_NC << 8) | (1);
    alarm_cb_args[1].msg.content.ptr = (void *)&sensor_02;

    alarm_cb_args[2].pid = thread_getpid();
    alarm_cb_args[2].msg.type = (SENSOR_REED_SWITCH_NO << 8) | (2);
    alarm_cb_args[2].msg.content.ptr = (void *)&sensor_02;

    reed_sensor_driver_params_t params = {.nc_pin = nc_pin,
                                          .no_pin = no_pin,
                                          .nc_int_flank = GPIO_BOTH,
                                          .no_int_flank = GPIO_BOTH,
                                          .nc_callback = reed_nc_callback,
                                          .no_callback = reed_no_callback,
                                          .nc_callback_args = (void *)&alarm_cb_args[1],
                                          .no_callback_args = (void *)&alarm_cb_args[2],
                                          .use_external_pulldown = false,
                                          .debounce_ms = REED_SENSOR_DEBOUNCE_MS};

    reed_sensor_driver_init(&sensor_02, &params);

    // Set up alarms / irqs for all the sensors (i.e. decide whether they shall be called via timer isr, gpio isr etc)
    alarm_cb_args[0].pid = thread_getpid();
    alarm_cb_args[0].msg.type = (SENSOR_DWAX509M183X0 << 8) | (0);
    alarm_cb_args[0].msg.content.ptr = (void *)&sensor_01;

    // // Get a timestamp in one hour
    alarm_timer.callback = dwax_alarm_cb;
    alarm_timer.arg = &alarm_cb_args[0];
    ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);


    static int event_counter = 0;
    static int seq_num = 0;

    int sensor_data[NUM_SENSORS] = {0};


    msg_t msg;
    while (true)
    {
        msg_receive(&msg);
        uint8_t const sensor_type = msg.type >> 8;
        uint8_t const sensor_id = msg.type & 0x00FF;

        switch (sensor_type)
        {
        case SENSOR_DWAX509M183X0:
            (void)sensor_id;
            dwax509m183x0_t *dev = (dwax509m183x0_t *)msg.content.ptr;
            int distance_um = dwax509m183x0_distance_um(dev);

            ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);

            sensor_data[sensor_id] = distance_um;
            event_counter++;

            break;
        case SENSOR_REED_SWITCH_NC:
            reed_sensor_driver_t *reed_nc = (reed_sensor_driver_t *)msg.content.ptr;
            reed_sensor_val_t nc_val;
            reed_sensor_driver_read_nc(reed_nc, &nc_val);
            sensor_data[sensor_id] = nc_val;
            event_counter++;

            break;
        case SENSOR_REED_SWITCH_NO:
            reed_sensor_driver_t *reed_no = (reed_sensor_driver_t *)msg.content.ptr;
            reed_sensor_val_t no_val;
            reed_sensor_driver_read_no(reed_no, &no_val);
            sensor_data[sensor_id] = no_val;
            event_counter++;

            break;
        }
        uint8_t cbor_buf[CBOR_BUF_SIZE];
        if (encode_data(cbor_buf, CBOR_BUF_SIZE, sensor_data, NUM_SENSORS, event_counter, seq_num) == 0)
        {
            send_data(cbor_buf, CBOR_BUF_SIZE);
            seq_num++;
            
            if (ENABLE_DEBUG)
            {
                for (size_t i = 0; i < CBOR_BUF_SIZE; i++)
                {
                    printf("%02X", cbor_buf[i]);
                }
                printf("\n");
            }
        }
    }

    return 0;
}
