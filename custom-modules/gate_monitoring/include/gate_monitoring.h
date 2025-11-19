#pragma once

#include "stdbool.h"

/**
 * @brief Initializes the configured gate monitoring hardware and LoRaWAN module.
 * 
 * @retval  0 on success
 * @retval -1 on failure
 */
int initialize_monitoring(bool start_lorawan);


/**
 * @brief Starts the monitoring routine in a thread.
 * 
 * @retval  0 on success
 * @retval -1 on failure
 */
int start_monitoring_routine(void);
