#pragma once

#include "state_validation.h"

#include <stddef.h>
#include <stdint.h>

#define AGGREGATE_DATA                          0



/**
 * @brief Send the data via the implemented network stack.
 *
 * @param cbor_buf the cbor encoded data to send
 * @param buf_size size of the cbor buffer
 */
void send_data(const gate_state_t state, const uint32_t timestamp);

