#pragma once

#include "../../../src/event_processing.h"
#include "../../../src/state_validation.h"

#define PHASE_CLOSING true
#define PHASE_OPENING false

#define FIRST_SENSOR_VALUE_ID  0
#define MIDDLE_SENSOR_VALUE_ID 1
#define LAST_SENSOR_VALUE_ID   2

/* 
* Scenario 1: Valid closing sequence
*/
static gate_state_t sc1_state_closing_valid = {
    .gate_closed = false,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 1,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 2,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 3,
            .is_masked = false,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 2,

};

/* 
* Scenario 2: Valid opening sequence
*/
static gate_state_t sc2_state_opening_valid = {
    .gate_closed = true,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 3,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 2,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 1,
            .is_masked = false,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 0,

};

/* 
* Scenario 3: Valid closing sequence with the first sensor 'masked' and with a 'wrong' value.
*/
static gate_state_t sc3a_state_closing_valid_first_masked = {
    .gate_closed = false,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 1,
            .is_masked = true,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 2,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 3,
            .is_masked = false,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 2,

};

/* 
* Scenario 3b: Valid closing sequence with the 'middle' sensor 'masked' and with a 'wrong' value.
*/
static gate_state_t sc3b_state_closing_valid_middle_masked = {
    .gate_closed = false,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 1,
            .is_masked = true,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 2,
            .is_masked = true,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 3,
            .is_masked = false,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 2,

};


/* 
* Scenario 3c: Valid closing sequence with the last sensor 'masked' and with a 'wrong' value.
*/
static gate_state_t sc3c_state_closing_valid_last_masked = {
    .gate_closed = false,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 1,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 2,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 3,
            .is_masked = true,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 2,

};

/* 
* Scenario 4a: Valid opening sequence with the 'last' sensor 'masked' and with a 'wrong' value.
*/
static gate_state_t sc4a_state_opening_valid_last_masked = {
    .gate_closed = true,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 1,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 0,
            .event_counter = 1,
            .latest_arrive_ticket = 2,
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 3,
            .is_masked = false,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 0,

};


/* 
* Scenario 5a: Invalid closing sequence - first triggered last  
* - The correct sequence (first, middle, last) was not followed 
* -> middle and last sensor are out-of-sequence.
*/
static gate_state_t sc5a_state_closing_invalid_first_triggered_last = {
    .gate_closed = false,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 3, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 1, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 2, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 0,
};



/* 
* Scenario 5b: Invalid closing sequence - first sensor triggered as second (followed by the last)
*/
static gate_state_t sc5b_state_closing_invalid_first_triggered_second = {
    .gate_closed = false,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 2, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 1, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 0,
            .event_counter = 0,
            .latest_arrive_ticket = 0, /* <--  triggered after first ASSERT */
            .is_masked = true,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 0,
};

/* 
* Scenario 5c: Invalid closing sequence - middle sensor triggers last -> last is invalid
* Step 1: first and last are triggered in order -> valid
* Step 2: middle one triggers -> last is invalid
*/
static gate_state_t sc5c_state_closing_invalid_middle_out_of_seq = {
    .gate_closed = false,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 1, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 0,
            .event_counter = 0,
            .latest_arrive_ticket = 0, /* <-- set after first ASSERT*/
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 1,
            .event_counter = 0,
            .latest_arrive_ticket = 2, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 2,
};


/* 
* Scenario 6: partial opening and closing again
* Step 1: last and middle are triggered for opening
* Step 2: middle triggers for closing
*/
static gate_state_t sc6_state_partial_opening_closing = {
    .gate_closed = true,
    .sensor_value_states = {
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 0,
            .value_id = 0,
            .value = 1,
            .event_counter = 1,
            .latest_arrive_ticket = 1, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 1,
            .value_id = 1,
            .value = 0,
            .event_counter = 0,
            .latest_arrive_ticket = 5, /* <-- set to 6 after first ASSERT*/
            .is_masked = false,
            .is_out_of_sequence = false,
        },
        (sensor_value_state_t){
            .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
            .sensor_id = 2,
            .value_id = 2,
            .value = 0,
            .event_counter = 0,
            .latest_arrive_ticket = 4, /* <-- */
            .is_masked = false,
            .is_out_of_sequence = false,
        } },

    .sensor_mode = MAJORITY_SEQUENCE,
    .sensor_triggered_states = { false, false, false },
    .latest_value_id = 1,
};

