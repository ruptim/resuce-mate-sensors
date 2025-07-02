#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "cbor.h"
#include <stdint.h>

// - sensor data
#define CBOR_BUF_SIZE 40
#define AGGREGATE_DATA 1

/**
 * @brief Encodes the given sensor data array in cbor.
 *
 * @param buf           cbor buffer for the encoded data
 * @param buf_size      len of the cbor buffer
 * @param data          sensor data array
 * @param data_len      len of the sensor data array
 * @param event_counter current number of occurred events
 * @param seq_num       current sequence number
 * @return              -1 when failing to encode the data.
 */
int encode_data(uint8_t *buf, size_t buf_size, int *data, int data_len, int event_counter, int seq_num);

/**
 * @brief Send the data via the implemented network stack.
 *
 * @param cbor_buf the cbor encoded data to send
 * @param buf_size size of the cbor buffer
 */
void send_data(uint8_t *cbor_buf, size_t buf_size);

#endif // MESSAGES_H_
