/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Interaction between the application layer and the two types of drivers being evaluated for the rescue mate project
 *
 * @author      Jan Thies <jan.thies@haw-hamburg.de>
 *
 * @}
 */

#include "event_processing.h"
#include "periph_cpu_common.h"
#include <stdio.h>
#include <sys/unistd.h>
#include <time.h>
#include <string.h>

#include "board.h"
#include "msg.h"
#include "thread.h"
#include "ztimer.h"

#include "cbor.h"

/* driver headers */
#include "dwax509m183x0.h"

#include "reed_sensor_driver.h"

/* Application headers */

#include "sensor_config.h"
#include "messages.h"
#include "lora_networking.h"

/* Configure run parameters */


// - debug output
#define ENABLE_DEBUG 1
#include "debug.h"


/* ------------------------ */

int main(void)
{
    

    ztimer_acquire(ZTIMER_SEC);
    ztimer_sleep(ZTIMER_SEC, 4);
    ztimer_release(ZTIMER_SEC);

    puts("Application 'Silenos' starting.");

    if(init_sensors() != 0){
        puts("Sensors failed to initialized!");
        return -1;
    }
    


    
    // if(init_lorawan_stack() != 0){
    //     puts("LoRaWan failed to initialized!");
    //     return -1;
    // }
        
    puts("Receiving Events!");
    await_sensor_events();


    return 0;
}
