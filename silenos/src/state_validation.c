#include "state_validation.h"
#include "messages.h"
#include "sensor_config.h"
#include <stdio.h>
#include <stdlib.h>

gate_state_t gate_state;

gate_state_t gate_state_sending;

bool init_phase;




void init_gate_state(void)
{
    init_phase = true;
    gate_state.sensor_mode = ACTIVE_MULTI_SENSOR_MODE;
    gate_state.state = GATE_OPEN;

    /* trigger all sensors once to determine current status */
    for (size_t i = 0; i < NUM_UNIQUE_SENSOR_VALUES; i++) {
      

        const sensor_type_t sensor_type = DECODE_SENSOR_TYPE(alarm_cb_args[i].msg.type);
        const sensor_id_t sensor_id = DECODE_SENSOR_ID(alarm_cb_args[i].msg.type);
        gate_state.sensor_states[i].sensor_id = sensor_id;
        gate_state.sensor_states[i].sensor_id = sensor_type;

        msg_send(&alarm_cb_args[i].msg, alarm_cb_args[i].pid);
    }

}


void snapshot_current_gate_state(void){
    gate_state_sending = gate_state;
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

    ztimer_acquire(ZTIMER_USEC);
    ztimer_now_t timestamp = ztimer_now(ZTIMER_USEC);
    ztimer_release(ZTIMER_USEC);


    send_data(gate_state_sending, timestamp);

}
