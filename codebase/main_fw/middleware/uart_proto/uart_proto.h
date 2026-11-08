/**
  * @file      uart_proto.h
  *
  * @brief     //TODO
  *
  */

/** \addtogroup uart_proto
*  @{
*/

#ifndef UART_PROTO_H
#define UART_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "ring_buffer.h"
#include "uart_proto_cfg.h"
#include "uart_proto_def.h"



/******************************************************************************
 * DEFINES
 ******************************************************************************/

// None.



/******************************************************************************
 * PUBLIC TYPES
 ******************************************************************************/

// Raw frame after successful parsing.
typedef struct
{
    uint8_t  command;                         // Command code.
    uint16_t len;                             // Payload length (0..MAX).
    uint8_t  payload[UART_PROTO_PAYLOAD_MAX]; // Payload data.
    uint16_t crc;                             // Received CRC16.
} uart_proto_frame_t;



/******************************************************************************
 * INLINE FUNCTIONS
 ******************************************************************************/

// None.



/******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES
 ******************************************************************************/

// Assign the RX ring buffer. Must be called before UART_PROTO_ParsePoll.
uart_proto_status_t UART_PROTO_SetRingBuffer(ring_buf_t *p_ring);

// Run one step of the parsing state machine.
uart_proto_status_t UART_PROTO_ParsePoll(bool *const p_received);

// Get the last successfully parsed frame.
const uart_proto_frame_t *UART_PROTO_GetParsedFrame(void);

// Reset the parsing FSM.
void UART_PROTO_Reset(void);

// Compute the frame CRC16 (start_marker .. end of payload).
uint16_t UART_PROTO_CalcCrc16(const uint8_t *p_data,
                              uint16_t len);

// Build a frame into the output buffer (for transmission).
uart_proto_status_t UART_PROTO_BuildFrame(uint8_t cmd,
                                          const uint8_t *p_payload,
                                          uint16_t len,
                                          uint8_t *p_out,
                                          uint16_t out_size,
                                          uint16_t *p_out_len);



/******************************************************************************
 * END OF HEADER'S CODE
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // UART_PROTO_H

/** @}*/
