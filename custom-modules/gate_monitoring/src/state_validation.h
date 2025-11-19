#pragma once

#include "sensor_config.h"

#include <stdbool.h>
#include <stdint.h>

#define GATE_OPEN   0
#define GATE_CLOSED 1

typedef struct {
    sensor_type_t type;
    sensor_id_t sensor_id;
    sensor_id_t value_id;
    uint32_t value; // TODO: has to me more than 8 to support dwax 
    uint8_t event_counter;
    ztimer_now_t latest_arrive_time;
    bool is_masked;
} sensor_value_state_t;

typedef struct {
    bool state;
    sensor_value_state_t sensor_value_states[NUM_UNIQUE_SENSOR_VALUES];
    multi_sensor_mode_t sensor_mode;
    bool sensor_triggered_states[NUM_UNIQUE_SENSOR_VALUES];
    uint8_t activated_sensor_value_count;

} gate_state_t;

extern gate_state_t gate_state;

void init_gate_state(void);

void verify_gate_state(bool new_gate_state_value);

void snapshot_current_gate_state(void);


