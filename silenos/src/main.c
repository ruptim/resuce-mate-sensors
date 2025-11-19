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

#include "periph_cpu_common.h"
#include <stdio.h>
#include <sys/unistd.h>
#include <time.h>
#include <string.h>

#include "board.h"
#include "msg.h"
#include "thread.h"
#include "ztimer.h"




/* Application headers */

#include "gate_monitoring.h"



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


    initialize_monitoring(false);

    start_monitoring_routine();


    while (1)
    {
        thread_sleep();
        // ztimer_sleep(ZTIMER_MSEC, 1000);
    };
    

    return 0;
}
