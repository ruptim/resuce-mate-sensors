#include "data_eval.h"
#include "sensor_config.h"


#include "ztimer.h"
#include <stdio.h>


#define RCV_QUEUE_SIZE 4
static msg_t rcv_queue[RCV_QUEUE_SIZE];

#define TEMPORAL_CONFIRM_TIMER_INTERVAL_MS 1000
ztimer_t temporal_confirm_timer;

static int sensor_event_counter = 0;

uint8_t sensor_states[NUM_UNIQUE_SENSOR_VALUES] = {0};


void temporal_confirm_timer_callback(void *args){
    (void) args;
    printf("--Timer--\n");
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        printf("%d, ", sensor_states[i]);
    }
    printf("\n");
}


void new_sensor_event(uint8_t sensor_id, int value){
    sensor_states[sensor_id] = value;
    sensor_event_counter++;
    ztimer_set(ZTIMER_MSEC, &temporal_confirm_timer, TEMPORAL_CONFIRM_TIMER_INTERVAL_MS);
    printf("Sensor %d: %d\n", sensor_id, value);
}


void await_sensor_events(void){

    msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);


    // initialize timer callback
    temporal_confirm_timer.callback = temporal_confirm_timer_callback;

    msg_t msg;
    while (true) {
        msg_receive(&msg);

        const uint8_t sensor_type = msg.type >> 8;
        const uint8_t sensor_id = msg.type & 0x00FF;
        switch (sensor_type) {
        case SENSOR_TYPE_ID_DWAX509M183X0:
            (void)sensor_id;
            dwax509m183x0_t *dev = (dwax509m183x0_t *)msg.content.ptr;
            int distance_um = dwax509m183x0_distance_um(dev);

            new_sensor_event(sensor_id,distance_um);
            
            break;
            case SENSOR_TYPE_ID_REED_SWITCH_NC:
            (void)sensor_id;
            reed_sensor_driver_t *reed_nc = (reed_sensor_driver_t *)msg.content.ptr;
            reed_sensor_val_t nc_val;
            reed_sensor_driver_read_nc(reed_nc, &nc_val);
            
            new_sensor_event(sensor_id,nc_val);
            break;
        case SENSOR_TYPE_ID_REED_SWITCH_NO:
            (void)
                sensor_id; // not needed, but prevents the compiler from complaining about having declaration right after the "case label".
            reed_sensor_driver_t *reed_no = (reed_sensor_driver_t *)msg.content.ptr;
            reed_sensor_val_t no_val;
            reed_sensor_driver_read_no(reed_no, &no_val);
            
            new_sensor_event(sensor_id,no_val);

            break;
        }
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
    }
}


