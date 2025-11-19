#include "event_processing.h"
#include <stdio.h>
#include <sys/unistd.h>



/* driver headers */
#include "dwax509m183x0.h"

#include "reed_sensor_driver.h"

/* Application headers */

#include "sensor_config.h"
#include "lora_networking.h"



int initialize_monitoring(bool start_lorawan){

     if(init_sensors() != 0){
        puts("Sensors failed to initialized!");
        return -1;
    }
    


    if (start_lorawan){
        if(init_lorawan_stack() != 0){
        puts("LoRaWan failed to initialized!");
        return -1;
    }
    }
    
        
   
    return 0;
}



int start_monitoring_routine(void){
    puts("Receiving Events!");
    await_sensor_events();
    return 0;
}
