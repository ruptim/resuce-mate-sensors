#include "event_processing_seq_verification.h"

#include "embUnit/embUnit.h"
#include "tests_event_processing.h"

/* include sources to test */
#include "../../../src/event_processing.c"

static void set_up(void)
{
    // state_opening = (gate_state_t) {
    // .gate_closed = false,
    // . sensor_value_states = {
    //     (sensor_value_state_t){
    //         .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
    //         .sensor_id = 0,
    //         .value_id = 0,
    //         .value = 1,
    //         .event_counter = 1,
    //         .latest_arrive_ticket =  1,
    //         .is_masked = false,
    //         .is_out_of_sequence = false,
    //     },
    //     (sensor_value_state_t){
    //         .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
    //         .sensor_id = 1,
    //         .value_id = 1,
    //         .value = 1,
    //         .event_counter = 1,
    //         .latest_arrive_ticket =  2,
    //         .is_masked = false,
    //         .is_out_of_sequence = false,
    //     },
    //     (sensor_value_state_t){
    //         .type = SENSOR_TYPE_ID_REED_SWITCH_NO,
    //         .sensor_id = 2,
    //         .value_id = 2,
    //         .value = 1,
    //         .event_counter = 1,
    //         .latest_arrive_ticket =  3,
    //         .is_masked = false,
    //         .is_out_of_sequence = false,
    //     }
    // },

    // .sensor_mode = MAJORITY_SEQUENCE,
    // .sensor_triggered_states = {false, false, false},
    // .latest_value_id = 2,

    // };
}

static void tear_down(void)
{
}

static void test_sc1_closing_valid(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc1_state_closing_valid) == true);
}

static void test_sc2_opening_valid(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_OPENING, &sc2_state_opening_valid) == true);
}

static void test_sc3a_closing_valid_first_masked(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc3a_state_closing_valid_first_masked) == true);
}

static void test_sc3b_closing_valid_first_masked(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc3b_state_closing_valid_middle_masked) == true);
}

static void test_sc3c_closing_valid_last_masked(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_OPENING, &sc3c_state_closing_valid_last_masked) == true);
}

static void test_sc4a_opening_valid_last_masked(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc4a_state_opening_valid_last_masked) == true);
}

static void test_sc5a_state_closing_invalid_first_triggered_last(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc5a_state_closing_invalid_first_triggered_last) == false);
    
    TEST_ASSERT(sc5a_state_closing_invalid_first_triggered_last.sensor_value_states[1].is_out_of_sequence == true);
    TEST_ASSERT(sc5a_state_closing_invalid_first_triggered_last.sensor_value_states[2].is_out_of_sequence == true);
}


static void test_sc5b_state_closing_invalid_first_triggered_second(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc5b_state_closing_invalid_first_triggered_second) == false);

    /* test if middle sensor was flagged 'out of sequence' */
    TEST_ASSERT(sc5b_state_closing_invalid_first_triggered_second.sensor_value_states[1].is_out_of_sequence == true);

    /* simulate triggering of the last sensor and check for now valid sequence.  */
    sc5b_state_closing_invalid_first_triggered_second.sensor_value_states[2].latest_arrive_ticket = 3;
    
    sc5b_state_closing_invalid_first_triggered_second.sensor_value_states[2].value = REED_SENSOR_ACTIVATED;
    sc5b_state_closing_invalid_first_triggered_second.sensor_value_states[2].event_counter = 1;
    sc5b_state_closing_invalid_first_triggered_second.latest_value_id = 2;
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc5b_state_closing_invalid_first_triggered_second) == true);
}


static void test_sc5c_state_closing_invalid_middle_out_of_seq(void)
{
    set_up();

    /* check if sequence is valid */
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc5c_state_closing_invalid_middle_out_of_seq) == true);
     /* simulate triggering of the last sensor and check for now valid sequence.  */
    sc5c_state_closing_invalid_middle_out_of_seq.sensor_value_states[MIDDLE_SENSOR_VALUE_ID].latest_arrive_ticket = 3;
    sc5c_state_closing_invalid_middle_out_of_seq.sensor_value_states[MIDDLE_SENSOR_VALUE_ID].value = REED_SENSOR_ACTIVATED;
    sc5c_state_closing_invalid_middle_out_of_seq.sensor_value_states[MIDDLE_SENSOR_VALUE_ID].event_counter = 1;
    sc5c_state_closing_invalid_middle_out_of_seq.latest_value_id = MIDDLE_SENSOR_VALUE_ID;


    /* check if sequence is now invalid */
    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc5c_state_closing_invalid_middle_out_of_seq) == false);
    /* test if middle sensor was flagged 'out of sequence' */
    TEST_ASSERT(sc5c_state_closing_invalid_middle_out_of_seq.sensor_value_states[2].is_out_of_sequence == true);
    
}

static void test_sc6_state_partial_opening_closing(void)
{
    set_up();
    TEST_ASSERT(verify_ticket_sequence(PHASE_OPENING, &sc6_state_partial_opening_closing) == true);

    /* simulate middle sensor for closing and check for valid sequence.  */
    sc6_state_partial_opening_closing.sensor_value_states[MIDDLE_SENSOR_VALUE_ID].latest_arrive_ticket = 6;

    sc6_state_partial_opening_closing.sensor_value_states[MIDDLE_SENSOR_VALUE_ID].value = REED_SENSOR_ACTIVATED;
    sc6_state_partial_opening_closing.sensor_value_states[MIDDLE_SENSOR_VALUE_ID].event_counter = 1;
    sc6_state_partial_opening_closing.latest_value_id = MIDDLE_SENSOR_VALUE_ID;

    TEST_ASSERT(verify_ticket_sequence(PHASE_CLOSING, &sc6_state_partial_opening_closing) == true);
}


Test *tests_event_processing_seq_verification_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_sc1_closing_valid),
        new_TestFixture(test_sc2_opening_valid),
        new_TestFixture(test_sc3a_closing_valid_first_masked),
        new_TestFixture(test_sc3b_closing_valid_first_masked),
        new_TestFixture(test_sc3c_closing_valid_last_masked),
        new_TestFixture(test_sc4a_opening_valid_last_masked),
        new_TestFixture(test_sc5a_state_closing_invalid_first_triggered_last),
        new_TestFixture(test_sc5b_state_closing_invalid_first_triggered_second),
        new_TestFixture(test_sc5c_state_closing_invalid_middle_out_of_seq),
        new_TestFixture(test_sc6_state_partial_opening_closing),

    };

    EMB_UNIT_TESTCALLER(tests, .setUp = set_up, .tearDown = tear_down, fixtures);

    return (Test *)&tests;
}
