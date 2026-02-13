#include "state_validation.h"

#include "messages.h"
#include "sensor_config.h"
#include "sensors.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// - debug output
#define ENABLE_DEBUG 1
#include "debug.h"


#define STARTING_TICKET 0
static event_ticket_t event_ticket_counter = STARTING_TICKET;

/* the current gate state. */
gate_state_t gate_state;

/*  A snapshot of the current gate state. It is used for evaluating and sending the state, without it being changed by new events. */
static gate_state_t gate_state_snapshot;

static ztimer_now_t gate_state_snapshot_ts;

static event_ticket_t gate_state_snapshot_ticket;

/* Flag used to indicate intitialization phase, when there is no 'groundtruth'state to compare against. */
bool init_phase;

/**
 * @brief Check whether the sensor values are valid or show inconsistencies.
 *        What a inconsisten state is, depends on the kind of sensor.
 * 
 */
static void verify_sensors(void);

static bool verify_reed_sensor(sensor_value_state_t nc_state, sensor_value_state_t no_state);

event_ticket_t get_new_event_ticket(void)
{
    event_ticket_counter++;
    return event_ticket_counter;
}

void reset_event_ticket(void)
{
    event_ticket_counter = STARTING_TICKET;
}

event_ticket_t get_snapshot_event_ticket(void)
{
    return gate_state_snapshot_ticket;
}

void init_gate_state(void)
{
    init_phase = true;
    gate_state.sensor_mode = ACTIVE_MULTI_SENSOR_MODE;
    gate_state.gate_closed = GATE_OPEN;

    /* trigger all sensors once to determine current status */
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        const sensor_type_t sensor_type = DECODE_SENSOR_TYPE(alarm_cb_args[i].msg.type);
        const sensor_id_t sensor_id = DECODE_SENSOR_ID(alarm_cb_args[i].msg.type);
        const value_id_t value_id = DECODE_VALUE_ID(alarm_cb_args[i].msg.type);
        gate_state.sensor_value_states[i].sensor_id = sensor_id;
        gate_state.sensor_value_states[i].type = sensor_type;
        gate_state.sensor_value_states[i].value_id = value_id;

        gate_state.sensor_value_states[i].latest_arrive_ticket = 0;

        msg_send(&alarm_cb_args[i].msg, alarm_cb_args[i].pid);
    }
}

void snapshot_current_gate_state(void)
{
    gate_state_snapshot = gate_state;
    gate_state_snapshot_ticket = event_ticket_counter;
}

void verify_gate_state(bool new_gate_state_value, bool is_closing_phase)
{
     
    /* in the init phase there is no current known gate state to check against  */
    if (init_phase) {
        init_phase = false;
        new_gate_state_value = GATE_OPEN;
        // return;
    }
    if (gate_state_snapshot.gate_closed == new_gate_state_value) {
        //TODO; handle case when state is the same as before.
    }
    else {
        gate_state_snapshot.gate_closed = new_gate_state_value;
    }

    printf("[INFO] Phase is %s. New state: %s\n",is_closing_phase ? "CLOSING" : "OPENING",
                                              new_gate_state_value ? "CLOSED" : "OPEN"
                                            );



   
    verify_sensors();

    ztimer_acquire(ZTIMER_MSEC);
    gate_state_snapshot_ts = ztimer_now(ZTIMER_MSEC);
    ztimer_release(ZTIMER_MSEC);

    send_data(gate_state_snapshot, gate_state_snapshot_ts);
}
static bool verify_reed_sensor(sensor_value_state_t nc_state, sensor_value_state_t no_state)
{
    (void)nc_state;
    (void)no_state;

    return nc_state.value == no_state.value;
}

static void verify_sensors(void)
{
    bool sensor_fault[NUM_SENSORS] = {};

    size_t i = 0;
    DEBUG("[DEBG] Faults? ");
    while (i < NUM_UNIQUE_SENSOR_VALUES) {
        sensor_value_state_t cur_sensor_val = gate_state.sensor_value_states[i];
        switch (cur_sensor_val.type) {
        case SENSOR_TYPE_ID_REED_SWITCH_NC:
            /* if a corresponding NO pin exists, use it to verify the sensor. */
            if (i + 1 < NUM_UNIQUE_SENSOR_VALUES && cur_sensor_val.sensor_id == gate_state.sensor_value_states[i + 1].sensor_id) {
                sensor_fault[cur_sensor_val.sensor_id] = verify_reed_sensor(gate_state.sensor_value_states[i], gate_state.sensor_value_states[i + 1]);

                i++; /* skip the following NC state value */
            }
            break;
        case SENSOR_TYPE_ID_REED_SWITCH_NO:
            /* the order of NC and NO pins is fixed. if a corresponding NC pin exists, it was already caught at the previous case. */
            break;
        default:
            break;
        }
        DEBUG("ID: %d, %d | ", cur_sensor_val.sensor_id, sensor_fault[cur_sensor_val.sensor_id]);
        i++;
    }
    puts("");
}
