#include "event_processing.h"
#include "sensor_config.h"
#include "sensors.h"
#include "state_validation.h"

#include "mutex.h"

#include "ztimer.h"
#include <stdio.h>

// - debug output
#define ENABLE_DEBUG 1
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
 * @brief Verfiy the order in which the sensor events have arrived in.
 * 
 * @retval 1 the order is valid.
 * @retval 0 the order is invalid .
 */
static int verify_order(void);

/**
 * @brief Evalaute the multi_sensor_mode_t 'MAJORITY_ORDERED' requiring a majority
 *        defined by 'majority_threshold' and a valid event order.
 * 
 * @retval 1 if the gate is determined to be closed. 
 * @retval 0 if the gate is determined to be open. 
 */
static int eval_majority_ordered_mode(void);

/**
 * @brief Evalaute the multi_sensor_mode_t 'EQUAL_ORDERED' requiring all values to indicate the 
 *        same gate state and a valid event order.
 * 
 * @retval 1 if the gate is determined to be closed. 
 * @retval 0 if the gate is determined to be open. 
 */
static int eval_equal_ordered_mode(void);

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
        // gate_state.sensor_value_states[sensor_id].id = sensor_id;
        // gate_state.sensor_value_states[sensor_id].type = sensor_type;
        gate_state.sensor_value_states[value_id].latest_arrive_time = time;
        gate_state.sensor_value_states[value_id].value = value;
        gate_state.sensor_value_states[value_id].event_counter += 1;
        sensor_event_counter++;

        if (gate_state.sensor_value_states[value_id].event_counter == MAX_EVENTS_FOR_SENSOR_FAULT) {
            printf("[INFO] Sensor value with id %d reached maximum events. Possible fault or tampering.\n", value_id);
            // TODO report after timer expired
            gate_state.sensor_value_states[value_id].is_masked = true;
        }
        ztimer_set(ZTIMER_MSEC, &temporal_confirm_timer, TEMPORAL_CONFIRM_TIMER_INTERVAL_MS);

        DEBUG("Sensor %d (%s): %d, %lu\n", sensor_id,
               sensor_type == SENSOR_TYPE_ID_REED_SWITCH_NC ? "NC" : "NO", value, time);
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

    puts("[INFO] Receiving Events!");

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

bool compare_reed_sensor_value_state(sensor_value_state_t sensor, uint8_t comp_state)
{
    bool is_nc_pin = (sensor.type == SENSOR_TYPE_ID_REED_SWITCH_NC);
    bool check_is_activated = (comp_state == REED_SENSOR_ACTIVATED);

    if (check_is_activated) {
        return (is_nc_pin) ? (sensor.value == REED_SENSOR_PIN_STATE_OPEN) :
                             (sensor.value == REED_SENSOR_PIN_STATE_CLOSED);
    }
    else {
        return (is_nc_pin) ? (sensor.value == REED_SENSOR_PIN_STATE_CLOSED) :
                             (sensor.value == REED_SENSOR_PIN_STATE_OPEN);
    }
}

static int verify_order(void)
{
    //TODO: verifiy order for both phases (opening and closing)
    int order_valid = 1;
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        if (i > 0 ? (gate_state.sensor_value_states[i].latest_arrive_time > gate_state.sensor_value_states[i - 1].latest_arrive_time) : true) {
            order_valid &= 1;
        }
        else {
            order_valid = 0;
        }
    }
    return order_valid;
}

static int eval_equal_ordered_mode(void)
{
    int state = GATE_CLOSED;
    /* check if all value states are "activated" and update the sensor_triggered_states array */
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        if (compare_reed_sensor_value_state(gate_state.sensor_value_states[i], REED_SENSOR_ACTIVATED)) {
            state &= GATE_CLOSED;
            gate_state.sensor_triggered_states[i] = true;
        }
        else {
            state = GATE_OPEN;
            gate_state.sensor_triggered_states[i] = false;
        }
        gate_state.sensor_value_states[i].event_counter = 0;
    }
    return state & verify_order();
}

static int eval_majority_ordered_mode(void)
{
    int state = 0;
    /* check how many value states are "activated" and update the sensor_triggered_states array */
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        if (compare_reed_sensor_value_state(gate_state.sensor_value_states[i], REED_SENSOR_ACTIVATED)) {
            state++;
            gate_state.sensor_triggered_states[i] = true;
        }
        else if (gate_state.sensor_triggered_states[i]) {
            gate_state.sensor_triggered_states[i] = false;
        }

        gate_state.sensor_value_states[i].event_counter = 0;
    }

    /* are the majority of values in their "activated" state? */
    return (state >= majority_threshold) & verify_order();
}

void *evaluate_gate_state(void *arg)
{
    (void)arg;

    /* lock mutex so the sensor values don't change during evaluation. */
    mutex_lock(&gate_state_mutex);

    snapshot_current_gate_state();

    DEBUG("----\n");
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        DEBUG("%ld (%d), ", (long unsigned int)gate_state.sensor_value_states[i].value, gate_state.sensor_value_states[i].event_counter);
    }
    DEBUG("\n");

    int final_state = GATE_OPEN;

    /* sensor check for configuration of multiple equivalent (ordered) reed sensors */
    switch (gate_state.sensor_mode) {
    case EQUAL_ORDERED:
        final_state = eval_equal_ordered_mode();
        break;
    case MAJORITY_ORDERED:
        final_state = eval_majority_ordered_mode();
    default:
        break;
    }

    mutex_unlock(&gate_state_mutex);

    printf("[INFO] Gate State is: %s\n", final_state == GATE_CLOSED ? "closed" : "open");

    /* the next step is to verfiy the new gate state. */
    verify_gate_state(final_state);

    return NULL;
}
