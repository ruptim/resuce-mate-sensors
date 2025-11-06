
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
 * @brief   Send a LoRaWAN packet with temperature data.
 * @param   netif       Pointer to the LoRaWAN network interface.
 * @param   cbor_buf    Pointer to the cbor data to be sent.
 * @param   buf_size    Length of the cbor data to be sent.
 *
 * @retval   0 on success
 * @retval  -1 on failure
 */
int send_lorawan_packet(const netif_t *netif, uint8_t *cbor_buf, size_t buf_size);
