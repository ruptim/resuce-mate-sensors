#include "event_processing.h"
#include "sensor_config.h"
#include "sensors.h"
#include "state_validation.h"

#include "mutex.h"

#include "ztimer.h"
#include <stdint.h>
#include <stdio.h>

#include "assert.h"

// - debug output

#define ENABLE_DEBUG GATE_MONITORING_ENABLE_DEBUG
#include "debug.h"

#define MAX_EVENTS_FOR_SENSOR_FAULT 200

/* ------------ variable declarations ---------------- */

static const int majority_threshold = ((NUM_UNIQUE_SENSOR_VALUES & 0x1) == 0x0 ? NUM_UNIQUE_SENSOR_VALUES >> 0x1 : (NUM_UNIQUE_SENSOR_VALUES >> 0x1) + 1);

#define RCV_QUEUE_SIZE 8
static msg_t rcv_queue[RCV_QUEUE_SIZE];

static char eval_thread_stack[THREAD_STACKSIZE_MAIN];

/* defines the time to wait when a new event has arrived after which the sensor values are evaluated */
#define TEMPORAL_CONFIRM_TIMER_INTERVAL_MS 2000
static ztimer_t temporal_confirm_timer = { 0 };

static long long sensor_event_counter = 0;

static mutex_t gate_state_mutex = MUTEX_INIT;

/* ------------ Prototype declarations ---------------- */

/**
 * @brief Timer callback triggering when TEMPORAL_CONFIRM_TIMER_INTERVAL_MS has passed, starting a thread to evaluate the sensor values.
 * 
 * @param args  not used.
 */
static void temporal_confirm_timer_callback(void *args);

/**
 * @brief Add a new sensor event to the current gate state sensor values. 
 * 
 * @param sensor_id     Sensor ID from the the received event.
 * @param sensor_type   Sensor type from the the received event.
 * @param value_id      Value ID from the the received event.
 * @param value         Sensor value to add.
 */
static void new_sensor_event(uint8_t sensor_id, uint8_t sensor_type, uint8_t value_id, int value);

/**
 * @brief Resets the event tickets of all sensors. New tickets are assigned to the sensors in sequence, 
 *        starting with the STARTING_TICKET for the ‘first’ in the current phase.
 *        CLOSING -> Start Index=0, OPENING -> Start Index = NUM_UNIQUE_SENSOR_VALUES-1
 * 
 * @param is_closing_phase  the current phase.    
 */
static void reset_sensor_tickets(bool is_closing_phase);

/**
 * @brief Compares a reed sensors value (state) to a given state.
 * 
 * @param sensor_value  The sensor value to check.    
 * @param comp_state    The state to compare against (REED_SENSOR_ACTIVATED or REED_SENSOR_NOT_ACTIVATED)
 * @retval true         Sensor state equals the requested state.
 * @retval false        Sensor state doesn't equals the requested state.
 */
static bool compare_reed_sensor_value_state(sensor_value_state_t sensor_value, uint8_t comp_state);

/**
 * @brief Routine to evaluate the current sensor values to determine the physical gate state (opend or closed). 
 * 
 * @param arg       not used.
 */
static void *evaluate_gate_state(void *arg);

/**
 * @brief Verfiy the sequence in which 'valid' sensor events have arrived in based on their tickets.  
 *        Valid are events of sensors that are not masked, and with tickets > than the latest snapshot ticket.
 *        It's not about verifing wether all sensor have the same value. 
 * 
 * @retval 1 the sequence is valid.
 * @retval 0 the sequence is invalid .
 */
static bool verify_ticket_sequence(bool closing_phase, gate_state_t *cur_gate_state);

/**
 * @brief Evalaute the multi_sensor_mode_t 'MAJORITY_SEQUENCE' requiring a majority
 *        defined by 'majority_threshold' and a valid event sequence.
 * 
 * @retval 1 if the gate is determined to be closed. 
 * @retval 0 if the gate is determined to be open. 
 */
static int eval_majority_sequence_mode(bool closing_phase);

/**
 * @brief Evalaute the multi_sensor_mode_t 'EQUAL_SEQUENCE' requiring all values to indicate the 
 *        same gate state and a valid event sequence.
 * 
 * @retval 1 if the gate is determined to be closed. 
 * @retval 0 if the gate is determined to be open. 
 */
static int eval_equal_sequence_mode(bool closing_phase);

/* ------------ function definitions ---------------- */

void temporal_confirm_timer_callback(void *args)
{
    (void)args;

    thread_create(eval_thread_stack, sizeof(eval_thread_stack), THREAD_PRIORITY_MAIN - 1, 0,
                  evaluate_gate_state, NULL, "gate_eval_thread");
}

void new_sensor_event(uint8_t sensor_id, uint8_t sensor_type, uint8_t value_id, int value)
{
    ztimer_acquire(ZTIMER_USEC);
    ztimer_now_t time = ztimer_now(ZTIMER_USEC);
    ztimer_release(ZTIMER_USEC);

    mutex_lock(&gate_state_mutex);

    if (!gate_state.sensor_value_states[value_id].is_masked) {
        event_ticket_t ticket = get_new_event_ticket();
        // gate_state.sensor_value_states[sensor_id].id = sensor_id;
        // gate_state.sensor_value_states[sensor_id].type = sensor_type;
        gate_state.sensor_value_states[value_id].latest_arrive_ticket = ticket;
        gate_state.sensor_value_states[value_id].latest_arrive_time_ms = time;
        gate_state.sensor_value_states[value_id].value = value;
        gate_state.sensor_value_states[value_id].event_counter += 1;
        gate_state.sensor_value_states[value_id].is_out_of_sequence = false;
        gate_state.latest_value_id = value_id;

        sensor_event_counter++;

        if (gate_state.sensor_value_states[value_id].event_counter == MAX_EVENTS_FOR_SENSOR_FAULT) {
            DEBUG("[INFO] Sensor value with id %d reached maximum events. Possible fault or tampering.\n", value_id);
            // TODO report after timer expired
            gate_state.sensor_value_states[value_id].is_masked = true;
        }
        ztimer_set(ZTIMER_MSEC, &temporal_confirm_timer, TEMPORAL_CONFIRM_TIMER_INTERVAL_MS);

        DEBUG("[DEBG] Sensor %d (%s): %d, %d\n", sensor_id,
              sensor_type == SENSOR_TYPE_ID_REED_SWITCH_NC ? "NC" : "NO", value, ticket);
    }

    mutex_unlock(&gate_state_mutex);
}

void *await_sensor_events(void *arg)
{
    (void)arg;
    msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);

    /* initialize timer callback */
    temporal_confirm_timer.callback = temporal_confirm_timer_callback;

    /* initialize gate state by triggering all sensors once */
    init_gate_state();

    DEBUG("[INFO] Receiving Events!\n");

    while (true) {
        msg_t msg;
        msg_receive(&msg);

        const sensor_type_t sensor_type = DECODE_SENSOR_TYPE(msg.type);
        const sensor_id_t sensor_id = DECODE_SENSOR_ID(msg.type);
        const value_id_t value_id = DECODE_VALUE_ID(msg.type);

        switch (sensor_type) {
        // case SENSOR_TYPE_ID_DWAX509M183X0:
        //     (void)sensor_id;
        //     dwax509m183x0_t *dev = (dwax509m183x0_t *)msg.content.ptr;
        //     int distance_um = dwax509m183x0_distance_um(dev);

        //     new_sensor_event(sensor_id, sensor_type, value_id, distance_um);

        //     break;
            case SENSOR_TYPE_ID_REED_SWITCH_NC:
                (void)sensor_id;
                reed_sensor_driver_t *reed_nc = (reed_sensor_driver_t *)msg.content.ptr;
                reed_sensor_val_t nc_val;
                reed_sensor_driver_read_nc(reed_nc, &nc_val);

                new_sensor_event(sensor_id, sensor_type, value_id, nc_val);
                break;
            case SENSOR_TYPE_ID_REED_SWITCH_NO:
                (void)
                    sensor_id; // not needed, but prevents the compiler from complaining about having declaration right after the "case label".
                reed_sensor_driver_t *reed_no = (reed_sensor_driver_t *)msg.content.ptr;
                reed_sensor_val_t no_val;
                reed_sensor_driver_read_no(reed_no, &no_val);

                new_sensor_event(sensor_id, sensor_type, value_id, no_val);

                break;
            }
    }
}

static void reset_sensor_tickets(bool is_closing_phase){
    /* when in closing phase and all are triggered */
    uint8_t start_idx = is_closing_phase ? 0 : (NUM_UNIQUE_SENSOR_VALUES - 1);
    uint8_t end_idx = is_closing_phase ? NUM_UNIQUE_SENSOR_VALUES : -1;
    uint8_t step = is_closing_phase ? 1 : -1;

    uint8_t i = start_idx;

    reset_event_ticket();

    while (i != end_idx) {
        gate_state.sensor_value_states[i].latest_arrive_ticket = get_new_event_ticket();
        i += step;
    }

}


bool compare_reed_sensor_value_state(sensor_value_state_t sensor, uint8_t comp_state)
{
    bool is_nc_pin = (sensor.type == SENSOR_TYPE_ID_REED_SWITCH_NC);
    bool check_is_activated = (comp_state == REED_SENSOR_ACTIVATED);

    if (check_is_activated) {
        /* if the sensor is activated, the NC pin should be 0. The NO pin should be 1. */
        return (is_nc_pin) ? (sensor.value == REED_SENSOR_PIN_STATE_OPEN) :
                             (sensor.value == REED_SENSOR_PIN_STATE_CLOSED);
    }
    else {
        return (is_nc_pin) ? (sensor.value == REED_SENSOR_PIN_STATE_CLOSED) :
                             (sensor.value == REED_SENSOR_PIN_STATE_OPEN);
    }
}

bool verify_ticket_sequence(bool closing_phase, gate_state_t *cur_gate_state)
{
    int phase_comp_state = closing_phase ? REED_SENSOR_ACTIVATED : REED_SENSOR_NOT_ACTIVATED;
    bool seq_valid = true;

    /* iteration parameters based on phase */
    int8_t start_idx = closing_phase ? 0 : (NUM_UNIQUE_SENSOR_VALUES - 1);
    int8_t end_idx = closing_phase ? NUM_UNIQUE_SENSOR_VALUES : -1;
    int8_t step = closing_phase ? 1 : -1;

    int8_t i = start_idx;
    int8_t prev_valid_i = -1;
    bool first_not_masked_found = false;

    while (i != end_idx) {
        /* 1. Skip masked sensors and sensors already flagged as out-of-sequence */
        if (cur_gate_state->sensor_value_states[i].is_masked || cur_gate_state->sensor_value_states[i].is_out_of_sequence) {
            i += step;
            continue;
        }

        /* 2. Filter out Sensors not corresponding to phase. */
        if (!compare_reed_sensor_value_state(cur_gate_state->sensor_value_states[i], phase_comp_state)) {
            i += step;
            continue;
        }

        /* 3. save the first 'valid' sensor (triggered & not masked) */
        if (!first_not_masked_found) {
            first_not_masked_found = true;
            prev_valid_i = i;
            i += step;
            continue;
        }

        /* 4. check wether current sensor (i) was triggered before the previous valid sensor (prev_valid_i) => invalid  */
        if (cur_gate_state->sensor_value_states[i].latest_arrive_ticket < cur_gate_state->sensor_value_states[prev_valid_i].latest_arrive_ticket) {
            seq_valid = false;
            cur_gate_state->sensor_value_states[i].is_out_of_sequence = true;
            continue;
        }

        prev_valid_i = i;
        i += step;
    }

    return seq_valid;
}

static int eval_equal_sequence_mode(bool closing_phase)
{
    bool state = GATE_CLOSED;
    uint8_t state_counter = 0;

    verify_ticket_sequence(closing_phase, &gate_state);
    /* check if all value states are "activated" and update the sensor_triggered_states array */
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        /* a masked sensor is treated as not activated */
        if (!gate_state.sensor_value_states[i].is_masked && !gate_state.sensor_value_states[i].is_out_of_sequence &&
            compare_reed_sensor_value_state(gate_state.sensor_value_states[i], REED_SENSOR_ACTIVATED)) {
            state &= GATE_CLOSED;
            gate_state.sensor_triggered_states[i] = true;
            state_counter += 1;
        }
        else {
            state = GATE_OPEN;
            gate_state.sensor_triggered_states[i] = false;
        }
        gate_state.sensor_value_states[i].event_counter = 0;
    }

    /* flag wether all are activate or none is activated */
    if (state_counter == 0 || state_counter == NUM_UNIQUE_SENSOR_VALUES) {
        gate_state.all_sensor_in_same_state = true;
    }
    else {
        gate_state.all_sensor_in_same_state = false;
    }

    return state;
}

static int eval_majority_sequence_mode(bool closing_phase)
{
    int state_counter = 0;

    int phase_comp_state = closing_phase ? REED_SENSOR_ACTIVATED : REED_SENSOR_NOT_ACTIVATED;
    int phase_return_state = closing_phase ? GATE_CLOSED : GATE_OPEN;

    verify_ticket_sequence(closing_phase, &gate_state);
    /* check how many value states are corresponding to the current phase and update the sensor_triggered_states array */
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        /* a masked or out-of-seq. sensor is treated as not activated */
        if ((!gate_state.sensor_value_states[i].is_masked && !gate_state.sensor_value_states[i].is_out_of_sequence) &&
            compare_reed_sensor_value_state(gate_state.sensor_value_states[i], phase_comp_state)) {
            state_counter++;
            gate_state.sensor_triggered_states[i] = true;
        }
        else if (gate_state.sensor_triggered_states[i]) {
            gate_state.sensor_triggered_states[i] = false;
        }

        gate_state.sensor_value_states[i].event_counter = 0;
    }

    /* flag wether all are activate or none is activated */
    if (state_counter == 0 || state_counter == NUM_UNIQUE_SENSOR_VALUES) {
        gate_state.all_sensor_in_same_state = true;
    }
    else {
        gate_state.all_sensor_in_same_state = false;
    }
    

    DEBUG("[DEBG] MAJ_SEQ for %s: %d > %d ?\n", phase_return_state == GATE_CLOSED ? "CLOSED" : "OPEN", state_counter, majority_threshold);
    /* are the majority of values in their "activated" state? */
    return (state_counter >= majority_threshold) ? phase_return_state : !phase_return_state;
}

void *evaluate_gate_state(void *arg)
{
    (void)arg;

    /* lock mutex so the sensor values don't change during evaluation. */
    mutex_lock(&gate_state_mutex);


    DEBUG("[DEBG] ----\n[DEBG] State (triggers, ticket): ");
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        DEBUG("%ld (%d, %d), ", (long unsigned int)gate_state.sensor_value_states[i].value, gate_state.sensor_value_states[i].event_counter, gate_state.sensor_value_states[i].latest_arrive_ticket);
    }
    DEBUG("\n");



    bool gate_is_closed = GATE_OPEN;

    /* the current phase (OPENING or CLOSING) is defined by the state of the last triggered sensor */
    bool is_closing_phase = compare_reed_sensor_value_state(gate_state.sensor_value_states[gate_state.latest_value_id], REED_SENSOR_ACTIVATED);

    /* sensor check for configuration of multiple equivalent (sequence) reed sensors */
    switch (gate_state.sensor_mode) {
    case EQUAL_SEQUENCE:
        gate_is_closed = eval_equal_sequence_mode(is_closing_phase);
        break;
    case MAJORITY_SEQUENCE:
        gate_is_closed = eval_majority_sequence_mode(is_closing_phase);
    default:
        break;
    }


    /* If all sensors are in the same state, reset the event tickets to lower values to prevent a overflow in the longterm. */
    if (gate_state.all_sensor_in_same_state){
        reset_sensor_tickets(is_closing_phase);
    }

    /* save snapshot of current gate state for verification and sending   */
    snapshot_current_gate_state();



    /* the next step is to verfiy the new gate state. */
    verify_gate_state(gate_is_closed, is_closing_phase);


    mutex_unlock(&gate_state_mutex);


    return NULL;
}
