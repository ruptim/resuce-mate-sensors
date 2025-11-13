#include "state_validation.h"
#include "messages.h"
#include "sensor_config.h"
#include "sensors.h"
#include <stdio.h>
#include <stdlib.h>

gate_state_t gate_state;

gate_state_t gate_state_sending;

bool init_phase;



void verify_sensors(void);


void init_gate_state(void)
{
    init_phase = true;
    gate_state.sensor_mode = ACTIVE_MULTI_SENSOR_MODE;
    gate_state.state = GATE_OPEN;

    /* trigger all sensors once to determine current status */
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        const sensor_type_t sensor_type = DECODE_SENSOR_TYPE(alarm_cb_args[i].msg.type);
        const sensor_id_t sensor_id = DECODE_SENSOR_ID(alarm_cb_args[i].msg.type);
        const sensor_id_t value_id = DECODE_VALUE_ID(alarm_cb_args[i].msg.type);
        gate_state.sensor_states[i].sensor_id = sensor_id;
        gate_state.sensor_states[i].type = sensor_type;
        gate_state.sensor_states[i].value_id = value_id;

        msg_send(&alarm_cb_args[i].msg, alarm_cb_args[i].pid);
    }
}

void snapshot_current_gate_state(void)
{
    gate_state_sending = gate_state;
}

void verify_gate_state(bool new_gate_state)
{
    /* in the init phase there is no current known gate state to check against  */
    if (init_phase) {
        init_phase = false;
        gate_state.state = new_gate_state;
        /* return for the time being */
        return;
    }

    if (gate_state.state == new_gate_state) {
    }

    verify_sensors();


    ztimer_acquire(ZTIMER_USEC);
    ztimer_now_t timestamp = ztimer_now(ZTIMER_USEC);
    ztimer_release(ZTIMER_USEC);

    send_data(gate_state_sending, timestamp);
}
bool verify_reed_sensor(sensor_value_state_t nc_state, sensor_value_state_t no_state)
{
    (void)nc_state;
    (void)no_state;

    return nc_state.value == no_state.value;
}

void verify_sensors(void)
{
    bool sensor_fault[NUM_SENSORS] = {};

    size_t i = 0;
    while (i < NUM_UNIQUE_SENSOR_VALUES) {

        sensor_value_state_t cur_sensor_val = gate_state.sensor_states[i];
        switch (cur_sensor_val.type) {
        case SENSOR_TYPE_ID_REED_SWITCH_NC:
            /* if a corresponding NO pin exists, use it to verify the sensor. */
            if (i + 1 < NUM_UNIQUE_SENSOR_VALUES && cur_sensor_val.sensor_id == gate_state.sensor_states[i + 1].sensor_id) {
                sensor_fault[cur_sensor_val.sensor_id] = verify_reed_sensor(gate_state.sensor_states[i], gate_state.sensor_states[i + 1]);

                i++; /* skip the following NC state value */
            }
            break;
        case SENSOR_TYPE_ID_REED_SWITCH_NO:
            /* the order of NC and NO pins is fixed. if a corresponding NC pin exists, it was already caught at the previous case. */
            break;
        default:
            break;
        }
        printf("ID: %d, %d | ", cur_sensor_val.sensor_id, sensor_fault[cur_sensor_val.sensor_id]);
        i++;
    }
    puts("");
}   
