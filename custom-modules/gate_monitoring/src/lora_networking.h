#pragma once

#include <stdio.h>

#include "net/netif.h"


/**
 * @brief   Initialize lorawan network stack and join network.
 *
 * @retval   0 on success
 * @retval  -1 on failure
 */
int init_lorawan_stack(void);

/**
 * @brief   Notify the sending thread to send the requested data.
 * @param   cbor_buf    Pointer to the cbor data to be sent.
 * @param   buf_size    Length of the cbor data to be sent.
 *
 * @retval   0 on success
 * @retval  -1 on failure
 */
int notify_tx_thread(uint8_t *cbor_buf, size_t buf_size);
