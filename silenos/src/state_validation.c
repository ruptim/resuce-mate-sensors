#include "state_validation.h"
#include "sensor_config.h"
#include <stdio.h>
#include <stdlib.h>

gate_state_t gate_state;

void init_gate_state(void)
{
    gate_state.sensor_mode = ACTIVE_MULTI_SENSOR_MODE;
    gate_state.state = GATE_OPEN;

    /* trigger all sensors once to determine current status */
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
        const uint8_t sensor_id = alarm_cb_args[i].msg.type & 0x00FF;
        gate_state.sensor_states[i].sensor_id = sensor_id;

        msg_send(&alarm_cb_args[i].msg, alarm_cb_args[i].pid);
    }

}

void verify_gate_state(bool new_gate_state)
{
    if (gate_state.state == new_gate_state) {
    }
}
