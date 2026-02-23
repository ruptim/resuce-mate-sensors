/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     Silenos
 * @{
 *
 * @file
 * @brief       Silenos application main. Gate monitoring with configured sensors and reporting with LoRaWAN.
 *
 * @author      Jan Thies <jan.thies@haw-hamburg.de>, Timon Rupet <timon.rupelt@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <sys/unistd.h>

#include "thread.h"
#include "ztimer.h"

#include "periph/gpio.h"
#include "board.h"

/* Application headers */

#include "gate_monitoring.h"



// #define ENABLE_DEBUG 1
#include "debug.h"

#ifndef GATE_MONITORING_USE_LORAWAN
    #define GATE_MONITORING_USE_LORAWAN true
#endif

/* ------------------------ */

int main(void)
{
    
    ztimer_acquire(ZTIMER_SEC);
    ztimer_sleep(ZTIMER_SEC, 3);
    ztimer_release(ZTIMER_SEC);

    DEBUG("Application 'Silenos' starting.");


    initialize_monitoring(GATE_MONITORING_USE_LORAWAN);

    start_monitoring_routine();

    // gpio_toggle(LED1_PIN);
    while (1)
    {
        thread_sleep();
    };
    

    return 0;
}
