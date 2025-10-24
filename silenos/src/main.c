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

#include "data_eval.h"
#include <stdio.h>
#include <sys/unistd.h>
#include <time.h>
#include <string.h>

#include "board.h"
#include "msg.h"
#include "thread.h"
#include "ztimer.h"

#include "cbor.h"

/* driver headers */
#include "dwax509m183x0.h"

#include "reed_sensor_driver.h"

/* Application headers */

#include "sensor_config.h"
#include "messages.h"
#include "lora_networking.h"

/* Configure run parameters */

// - debug output
#define ENABLE_DEBUG 1
#include "debug.h"


/* ------------------------ */

int main(void)
{
    puts("RIOT application: 'Silenos'");
    

    init_sensors();

    await_sensor_events();

    // if(init_lora_stack() != 0){
    //     exit(EXIT_FAILURE);
    // }

    // static int event_counter = 0;
    // static int seq_num = 0;

    // int sensor_data[NUM_UNIQUE_SENSOR_VALUES] = { 0 };
    // int sensors_done = 0;
    // int all_sensors_done = (0x1 << NUM_UNIQUE_SENSOR_VALUES) -
    //                        1; // value when all are done (bit is 1) to compare against

    // msg_t msg;
    // while (true) {
    //     msg_receive(&msg);

    //     const uint8_t sensor_type = msg.type >> 8;
    //     const uint8_t sensor_id = msg.type & 0x00FF;
    //     switch (sensor_type) {
    //     case SENSOR_TYPE_ID_DWAX509M183X0:
    //         (void)sensor_id;
    //         dwax509m183x0_t *dev = (dwax509m183x0_t *)msg.content.ptr;
    //         int distance_um = dwax509m183x0_distance_um(dev);

    //         // ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);
    //         sensor_data[sensor_id] = distance_um;
    //         event_counter++;

    //         break;
    //     case SENSOR_TYPE_ID_REED_SWITCH_NC:
    //         (void)sensor_id;
    //         reed_sensor_driver_t *reed_nc = (reed_sensor_driver_t *)msg.content.ptr;
    //         reed_sensor_val_t nc_val;
    //         reed_sensor_driver_read_nc(reed_nc, &nc_val);
    //         sensor_data[sensor_id] = nc_val;
    //         event_counter++;
    //         break;
    //     case SENSOR_TYPE_ID_REED_SWITCH_NO:
    //         (void)
    //             sensor_id; // not needed, but prevents the compiler from complaining about having declaration right after the "case label".
    //         reed_sensor_driver_t *reed_no = (reed_sensor_driver_t *)msg.content.ptr;
    //         reed_sensor_val_t no_val;
    //         reed_sensor_driver_read_no(reed_no, &no_val);
    //         sensor_data[sensor_id] = no_val;
    //         event_counter++;

    //         break;
    //     }
    //     sensors_done |= 0x1 << sensor_id;

    //     // if AGGREGATE_DATA: only send if received the latest values from all sensors
    //     if ((AGGREGATE_DATA && sensors_done == all_sensors_done) || !AGGREGATE_DATA) {
    //         sensors_done = 0;

    //         uint8_t cbor_buf[CBOR_BUF_SIZE];
    //         if (encode_data(cbor_buf, CBOR_BUF_SIZE, sensor_data, NUM_UNIQUE_SENSOR_VALUES,
    //                         event_counter, seq_num) == 0) {
    //             send_data(cbor_buf, CBOR_BUF_SIZE);
    //             seq_num++;

    //             if (ENABLE_DEBUG) {
    //                 // for (size_t i = 0; i < CBOR_BUF_SIZE; i++)
    //                 // {
    //                 //     printf("%02X", cbor_buf[i]);
    //                 // }
    //                 // printf("\n");
    //                 for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
    //                     printf("%d, ", sensor_data[i]);
    //                 }
    //                 printf("\n");
    //             }
    //         }
    //     }
    // }

    return 0;
}
