#include "state_validation.h"
#include "sensor_config.h"
#include <stdio.h>
#include <stdlib.h>

gate_state_t gate_state;

bool init_phase;

void init_gate_state(void)
{
    init_phase = true;
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
    /* in the init phase there is no current known gate state to check against  */
    if (init_phase){
        
        init_phase = false;
        gate_state.state = new_gate_state;
        /* return for the time being */
        return ;
    }

    if (gate_state.state == new_gate_state) {

    }
}
