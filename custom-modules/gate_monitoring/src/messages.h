#pragma once

#include "state_validation.h"

#include <stddef.h>
#include <stdint.h>

#define AGGREGATE_DATA                          0


/**
 * @brief 
 * 
 * @param buf           cbor buffer for the encoded data
 * @param buf_size      len of the cbor buffer
 * @param gate_state    gate state with the data about sensors and gate to encode
 * @param seq_num       current sequence number
 * @param seq_num       local timestamp of time of sending 
 * @return              -1 when failing to encode the data.
 */
int encode_data(uint8_t *buf, size_t buf_size, gate_state_t gate_state, int seq_num, uint32_t timestamp_s);

/**
 * @brief Send the data via the implemented network stack.
 *
 * @param cbor_buf the cbor encoded data to send
 * @param buf_size size of the cbor buffer
 */
void send_data(const gate_state_t state, const uint32_t timestamp);

