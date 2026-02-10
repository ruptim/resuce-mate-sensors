#pragma once

#include "sensor_config.h"

#include <stdbool.h>
#include <stdint.h>

#define GATE_OPEN   false
#define GATE_CLOSED true


typedef uint16_t event_ticket_t;



/**
 * @brief An abstraction type for a sensors value. 
 * 
 */
typedef struct {
    sensor_type_t type;
    sensor_id_t sensor_id;
    value_id_t value_id;
    uint32_t value; 
    uint8_t event_counter;
    event_ticket_t latest_arrive_ticket;
    bool is_masked;
    bool is_out_of_sequence;
} sensor_value_state_t;

/**
 * @brief The gate state type consisting of all sensor value states, 
 *        the selected mode and the 'closed state' of the gate.
 * 
 */
typedef struct {
    bool gate_closed;
    sensor_value_state_t sensor_value_states[NUM_UNIQUE_SENSOR_VALUES];
    multi_sensor_mode_t sensor_mode;
    bool sensor_triggered_states[NUM_UNIQUE_SENSOR_VALUES];
    value_id_t latest_value_id;
    uint8_t activated_sensor_value_count;

} gate_state_t;

extern gate_state_t gate_state;

/**
 * @brief Get a new event ticket.  
 * 
 * @return event_ticket_t 
 */
event_ticket_t get_new_event_ticket(void);

/**
 * @brief Initialize the gate state struct and current sensor values
 *        by triggering the reading of all sensors. 
 * 
 */
void init_gate_state(void);

/**
 * @brief   Save a snapshot of the current gate state.
 * 
 */
void snapshot_current_gate_state(void);

/**
 * @brief Get the event ticket of the last snapshot.
 * 
 * @return event_ticket_t 
 */
event_ticket_t get_snapshot_event_ticket(void);

/**
 * @brief Verify the newly determined gate state against the current one and other verification mechanisms.
 *        If the state is valid, is is set as the new state and an update is send to the uplink.  
 *
 * @param new_gate_state_value The new gate state to verify. 
 */
void verify_gate_state(bool new_gate_state_value);


