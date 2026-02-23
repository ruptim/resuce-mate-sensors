#pragma once

#ifdef __cplusplus
extern C
{
#endif

    /**
 * @brief The event receive routine waiting for new sensor events. When a event arrives,
 *        the sensor type is determined to read it's current value and then forwards it 
 *        to be added to the current gate state.  
 * 
 */
    void *await_sensor_events(void *arg);

#ifdef __cplusplus
}
#endif
