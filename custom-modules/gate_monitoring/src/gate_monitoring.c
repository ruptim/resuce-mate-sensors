#include "gate_monitoring.h"
#include "event_processing.h"
#include "sensor_config.h"
#include "lora_networking.h"


#include <stdio.h>
#include <sys/unistd.h>
#include <errno.h>


static char monitoring_thread_stack[THREAD_STACKSIZE_MAIN];

static kernel_pid_t thread_pid;

int initialize_monitoring(bool start_lorawan){

    thread_pid = thread_create(monitoring_thread_stack, sizeof(monitoring_thread_stack), THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_SLEEPING,
                  await_sensor_events, NULL, "gate_monitor_thread");
    if (-EINVAL == thread_pid) {
        puts("[ERROR] Failed to create monitoring thread!");
        return -1;
    }

     if(init_sensors(thread_pid) != 0){
        puts("[ERROR] Sensors failed to initialized!");
        return -1;
    }
    


    if (start_lorawan){
        if(init_lorawan_stack() != 0){
        puts("[ERROR] LoRaWan failed to initialized!");
        return -1;
    }
    }
    
    return 0;
}



int start_monitoring_routine(void){

    puts("[INFO] Starting monitoring routine!");
    
    if (thread_wakeup(thread_pid) != 1) {
        puts("[ERROR] Failed to wake up monitoring thread!");
        return -1;
    }
    return thread_pid;
}
