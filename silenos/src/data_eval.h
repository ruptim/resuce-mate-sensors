#ifndef DATA_EVAL_H
#define DATA_EVAL_H


#include <stdint.h>

#ifdef __cplusplus
extern C {
#endif



void temporal_confirm_timer_callback(void *args);

void await_sensor_events(void);

void new_sensor_event(uint8_t sensor_id, int value);



#ifdef __cplusplus
}
#endif

#endif // DATA_EVAL_H
