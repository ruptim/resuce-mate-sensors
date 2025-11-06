#pragma once

#include "sensor_config.h"

#include <stdbool.h>
#include <stdint.h>

#define GATE_OPEN   0
#define GATE_CLOSED 1

typedef struct {
    uint8_t type;
    uint8_t sensor_id;
    uint8_t value;
    uint8_t event_counter;
    ztimer_now_t latest_arrive_time;
    bool is_masked;
} sensor_state_t;

typedef struct {
    bool state;
    sensor_state_t sensor_states[NUM_UNIQUE_SENSOR_VALUES];
    multi_sensor_mode_t sensor_mode;

} gate_state_t;

extern gate_state_t gate_state;

void init_gate_state(void);

void verify_gate_state(bool new_gate_state);
