// /*
//  * Copyright (C) 2024 HAW Hamburg
//  *
//  * This file is subject to the terms and conditions of the GNU Lesser
//  * General Public License v2.1. See the file LICENSE in the top level
//  * directory for more details.
//  */

// /**
//  * @ingroup     tests
//  * @{
//  *
//  * @file
//  * @brief       Interaction between the application layer and the two types of drivers being evaluated for the rescue mate project
//  *
//  * @author      Jan Thies <jan.thies@haw-hamburg.de>
//  *
//  * @}
//  */

// #include <stdio.h>
// #include <time.h>
// #include <string.h>

// #include "board.h"
// #include "msg.h"
// #include "sensor_config.h"
// #include "thread.h"
// #include "ztimer.h"

// #include "cbor.h"

// /* driver headers */
// #include "dwax509m183x0.h"
// #include "dwax509m183x0_params.h"

// #include "reed_sensor_driver.h"
// #include "reed_sensor_driver_params.h"

// /* Application headers */


// // #include "sensor_config.h"
// #include "sensors.h"
// #include "messages.h"
// #include "lora_networking.h"

// /* Configure run parameters */

// // - debug output
// #define ENABLE_DEBUG 1
// #include "debug.h"

// #define RCV_QUEUE_SIZE 4
// static msg_t rcv_queue[RCV_QUEUE_SIZE];


// gpio_t nc_pin = GPIO_PIN(1, 9); // D11
// gpio_t no_pin = GPIO_PIN(0, 8); // D13

// // - dwax timer
// #define ALARM_TIMER_INTERVAL_S 60
// ztimer_t alarm_timer;

// #define NUM_SENSORS 2

// #define NUM_UNIQUE_SENSOR_VALUES 4
// static alarm_cb_args_t alarm_cb_args[NUM_UNIQUE_SENSOR_VALUES];


// #define ENCODE_SENSOR_TYPE_ID(type, id) ((type) << 8 | (id))
// #define SENSOR_TYPE_ID_DWAX509M183X0 1
// #define SENSOR_TYPE_ID_REED_SWITCH_NC 2
// #define SENSOR_TYPE_ID_REED_SWITCH_NO 3

// #define REED_SENSOR_DEBOUNCE_MS 60

// /* ------------------------ */



// /* Sensor config */

// // static reed_sensor_driver_t sensor_01;
// // static reed_sensor_driver_t sensor_02;  // 2 "sensors" in one (normally-open, normally-closed)


// typedef union {
// 	reed_sensor_driver_t reed_sensor;
// 	dwax509m183x0_t inductive_sensor;
// } sensor_base_type2_t;

// typedef union {
// 	reed_sensor_driver_params_t reed_sensor_params;
// 	dwax509m183x0_params_t inductive_sensor_params;
// } sensor_base_params_t;

// sensor_base_type_t registered_sensors[NUM_SENSORS];
// sensor_base_params_t registered_sensors_params[NUM_SENSORS];
// #define SENSOR_1_REED_NC_ID2 0
// #define SENSOR_1_REED_NO_ID2 1

// #define SENSOR_2_REED_NC_ID2 2
// #define SENSOR_2_REED_NO_ID2 3

// int init_sensors2(void)
// {
//     alarm_cb_args[0].pid = thread_getpid();
//     alarm_cb_args[0].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC, 0);
//     alarm_cb_args[0].msg.content.ptr = (void *)&registered_sensors[0];

//     alarm_cb_args[1].pid = thread_getpid();
//     alarm_cb_args[1].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO, 1);
//     alarm_cb_args[1].msg.content.ptr = (void *)&registered_sensors[0];

//     // alarm_timer.callback = reed_nc_callback_and_dwax_trigger;
//     // alarm_timer.arg = &alarm_cb_args;
//     // ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);

//     registered_sensors_params[0] = (sensor_base_params_t) (reed_sensor_driver_params_t) {.nc_pin = nc_pin,
//                                           .no_pin = no_pin,
//                                           .nc_int_flank = GPIO_BOTH,
//                                           .no_int_flank = GPIO_BOTH,
//                                           //   .nc_callback = reed_nc_callback_and_dwax_trigger,
//                                           .nc_callback = reed_nc_callback,
//                                           .no_callback = reed_no_callback,
//                                         //   .nc_callback_args = (void *)&alarm_cb_args, // Note: passing the whole array to be able to access the callback of dawx too
//                                           .nc_callback_args = (void *)&alarm_cb_args[0],
//                                           .no_callback_args = (void *)&alarm_cb_args[1],
//                                           .use_external_pulldown = false,
//                                           .debounce_ms = REED_SENSOR_DEBOUNCE_MS};

//     // reed_sensor_driver_init(&sensor_01, &params);
//     reed_sensor_driver_init(&registered_sensors[0].reed_sensor, &registered_sensors_params[0].reed_sensor_params);

//     alarm_cb_args[2].pid = thread_getpid();
//     alarm_cb_args[2].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC, 2);
//     alarm_cb_args[2].msg.content.ptr = (void *)&registered_sensors[1];

//     alarm_cb_args[3].pid = thread_getpid();
//     alarm_cb_args[3].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO, 3);
//     alarm_cb_args[3].msg.content.ptr = (void *)&registered_sensors[1];

//     // alarm_timer.callback = reed_nc_callback_and_dwax_trigger;
//     // alarm_timer.arg = &alarm_cb_args;
//     // ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);

//     registered_sensors_params[1] = (sensor_base_params_t) (reed_sensor_driver_params_t){
//                                           .nc_pin = GPIO_PIN(0,6),
//                                           .no_pin = GPIO_PIN(0,27),
//                                           .nc_int_flank = GPIO_BOTH,
//                                           .no_int_flank = GPIO_BOTH,
//                                           //   .nc_callback = reed_nc_callback_and_dwax_trigger,
//                                           .nc_callback = reed_nc_callback,
//                                           .no_callback = reed_no_callback,
//                                         //   .nc_callback_args = (void *)&alarm_cb_args, // Note: passing the whole array to be able to access the callback of dawx too
//                                           .nc_callback_args = (void *)&alarm_cb_args[2],
//                                           .no_callback_args = (void *)&alarm_cb_args[3],
//                                           .use_external_pulldown = false,
//                                           .debounce_ms = REED_SENSOR_DEBOUNCE_MS};

//     // reed_sensor_driver_init(&sensor_02, &params2);
//     reed_sensor_driver_init(&registered_sensors[1].reed_sensor, &registered_sensors_params[1].reed_sensor_params);

//     return 0;
// }

// int main(void)
// {
//     puts("RIOT application: 'Silenos'");
//     msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);
//     memset(alarm_cb_args, 0, sizeof(alarm_cb_args));

//     init_sensors2();
//     // if(init_lora_stack() != 0){
//     //     exit(EXIT_FAILURE);
//     // }

//     // Initialize all connected sensors
//     // dwax509m183x0_init(&sensor_01, &dwax509m183x0_params[0]);

//     // Set up alarms / irqs for all the sensors (i.e. decide whether they shall be called via timer isr, gpio isr etc)
//     // alarm_cb_args[0].pid = thread_getpid();
//     // alarm_cb_args[0].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_DWAX509M183X0, 0); // encode sensor type
//     // alarm_cb_args[0].msg.content.ptr = (void *)&sensor_01;

//     // // Get a timestamp in one hour
//     // alarm_timer.callback = dwax_alarm_cb;
//     // alarm_timer.arg = &alarm_cb_args[0];

//     // alarm_cb_args[0].pid = thread_getpid();
//     // alarm_cb_args[0].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC, 0);
//     // alarm_cb_args[0].msg.content.ptr = (void *)&registered_sensors[0];

//     // alarm_cb_args[1].pid = thread_getpid();
//     // alarm_cb_args[1].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO, 1);
//     // alarm_cb_args[1].msg.content.ptr = (void *)&registered_sensors[0];

//     // // alarm_timer.callback = reed_nc_callback_and_dwax_trigger;
//     // // alarm_timer.arg = &alarm_cb_args;
//     // // ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);

//     // reed_sensor_driver_params_t params = {.nc_pin = nc_pin,
//     //                                       .no_pin = no_pin,
//     //                                       .nc_int_flank = GPIO_BOTH,
//     //                                       .no_int_flank = GPIO_BOTH,
//     //                                       //   .nc_callback = reed_nc_callback_and_dwax_trigger,
//     //                                       .nc_callback = reed_nc_callback,
//     //                                       .no_callback = reed_no_callback,
//     //                                     //   .nc_callback_args = (void *)&alarm_cb_args, // Note: passing the whole array to be able to access the callback of dawx too
//     //                                       .nc_callback_args = (void *)&alarm_cb_args[0],
//     //                                       .no_callback_args = (void *)&alarm_cb_args[1],
//     //                                       .use_external_pulldown = false,
//     //                                       .debounce_ms = REED_SENSOR_DEBOUNCE_MS};

//     // // reed_sensor_driver_init(&sensor_01, &params);
//     // reed_sensor_driver_init(&registered_sensors[0].reed_sensor, &params);

//     // alarm_cb_args[2].pid = thread_getpid();
//     // alarm_cb_args[2].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NC, 2);
//     // alarm_cb_args[2].msg.content.ptr = (void *)&registered_sensors[1];

//     // alarm_cb_args[3].pid = thread_getpid();
//     // alarm_cb_args[3].msg.type = ENCODE_SENSOR_TYPE_ID(SENSOR_TYPE_ID_REED_SWITCH_NO, 3);
//     // alarm_cb_args[3].msg.content.ptr = (void *)&registered_sensors[1];

//     // // alarm_timer.callback = reed_nc_callback_and_dwax_trigger;
//     // // alarm_timer.arg = &alarm_cb_args;
//     // // ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);

//     // reed_sensor_driver_params_t params2 = {.nc_pin = GPIO_PIN(0,6),
//     //                                       .no_pin = GPIO_PIN(0,27),
//     //                                       .nc_int_flank = GPIO_BOTH,
//     //                                       .no_int_flank = GPIO_BOTH,
//     //                                       //   .nc_callback = reed_nc_callback_and_dwax_trigger,
//     //                                       .nc_callback = reed_nc_callback,
//     //                                       .no_callback = reed_no_callback,
//     //                                     //   .nc_callback_args = (void *)&alarm_cb_args, // Note: passing the whole array to be able to access the callback of dawx too
//     //                                       .nc_callback_args = (void *)&alarm_cb_args[2],
//     //                                       .no_callback_args = (void *)&alarm_cb_args[3],
//     //                                       .use_external_pulldown = false,
//     //                                       .debounce_ms = REED_SENSOR_DEBOUNCE_MS};

//     // // reed_sensor_driver_init(&sensor_02, &params2);
//     // reed_sensor_driver_init(&registered_sensors[1].reed_sensor, &params2);

//     static int event_counter = 0;
//     static int seq_num = 0;

//     int sensor_data[NUM_UNIQUE_SENSOR_VALUES] = {0};
//     int sensors_done = 0;
//     int all_sensors_done = (0x1 << NUM_SENSORS) - 1;  // value when all are done (bit is 1) to compare against

//     msg_t msg;
//     while (true)
//     {
//     msg_receive(&msg);
//         uint8_t const sensor_type = msg.type >> 8;
//         uint8_t const sensor_id = msg.type & 0x00FF;
//         switch (sensor_type)
//         {
//         case SENSOR_TYPE_ID_DWAX509M183X0:
//             (void)sensor_id;
//             dwax509m183x0_t *dev = (dwax509m183x0_t *)msg.content.ptr;
//             int distance_um = dwax509m183x0_distance_um(dev);

//             // ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);
//             sensor_data[sensor_id] = distance_um;
//             event_counter++;

//             break;
//         case SENSOR_TYPE_ID_REED_SWITCH_NC:
//             (void) sensor_id;
//             // ztimer_set(ZTIMER_SEC, &alarm_timer, ALARM_TIMER_INTERVAL_S);
//             reed_sensor_driver_t *reed_nc = (reed_sensor_driver_t *)msg.content.ptr;
//             reed_sensor_val_t nc_val;
//             reed_sensor_driver_read_nc(reed_nc, &nc_val);
//             sensor_data[sensor_id] = nc_val;
//             event_counter++;
//             break;
//         case SENSOR_TYPE_ID_REED_SWITCH_NO:
//             (void)sensor_id; // not needed, but prevents the compiler from complaining about having declaration right after the "case label".
//             reed_sensor_driver_t *reed_no = (reed_sensor_driver_t *)msg.content.ptr;
//             reed_sensor_val_t no_val;
//             reed_sensor_driver_read_no(reed_no, &no_val);
//             sensor_data[sensor_id] = no_val;
//             event_counter++;

//             break;
//         }
//         sensors_done |= 0x1 << sensor_id;
//         // if AGGREGATE_DATA: only send if received the latest values from all sensors
//         if ((AGGREGATE_DATA && sensors_done == all_sensors_done) || !AGGREGATE_DATA)
//         {
//             sensors_done = 0;

//             uint8_t cbor_buf[CBOR_BUF_SIZE];
//             if (encode_data(cbor_buf, CBOR_BUF_SIZE, sensor_data, NUM_UNIQUE_SENSOR_VALUES, event_counter, seq_num) == 0)
//             {
//                 send_data(cbor_buf, CBOR_BUF_SIZE);
//                 seq_num++;

//                 if (ENABLE_DEBUG)
//                 {
//                     // for (size_t i = 0; i < CBOR_BUF_SIZE; i++)
//                     // {
//                     //     printf("%02X", cbor_buf[i]);
//                     // }
//                     // printf("\n");
//                     for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++)
//                     {
//                         printf("%d, ", sensor_data[i]);
//                     }
//                     printf("\n");
//                 }
//             }
//         }
//     }

//     return 0;
// }
