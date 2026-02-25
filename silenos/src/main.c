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


#include "shell.h"

#include "periph/pm.h"
/* Application headers */

#include "gate_monitoring.h"



// #define ENABLE_DEBUG 1
#include "debug.h"

#ifndef GATE_MONITORING_USE_LORAWAN
    #define GATE_MONITORING_USE_LORAWAN true
#endif

/* ------------------------ */


#define REBOOT_TIMER_INTERVAL_S (60*(60))*3 // 3H
static ztimer_t reboot_timer = { 0 };

void reboot_callback(void *args){
    (void) args;
    pm_reboot();
}


int main(void)
{


    reboot_timer.callback = reboot_callback;
    ztimer_set(ZTIMER_SEC, &reboot_timer, REBOOT_TIMER_INTERVAL_S);

    ztimer_acquire(ZTIMER_SEC);
    ztimer_sleep(ZTIMER_SEC, 3);
    ztimer_release(ZTIMER_SEC);

    DEBUG("Application 'Silenos' starting.");


    initialize_monitoring(GATE_MONITORING_USE_LORAWAN);

    start_monitoring_routine();

        /* Buffer to store command line input */
    char buffer[SHELL_DEFAULT_BUFSIZE];

    /* Start the shell */
    shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);

    // gpio_toggle(LED1_PIN);
    while (1)
    {
        thread_sleep();
    };
    

    return 0;
}
