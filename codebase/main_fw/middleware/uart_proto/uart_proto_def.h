/**
  * @file      uart_proto_cmd_def.h
  *
  * @brief     UART protocol command definitions.
  *
  */

/** \addtogroup UART_PROTO
*  @{
*/

#ifndef UART_PROTO_CMD_DEF_H
#define UART_PROTO_CMD_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

// DEFINES

// Frame start marker.
#define UART_PROTO_START_MARKER        (0xAAU)

// Frame end marker.
#define UART_PROTO_END_MARKER          (0x55U)

// Maximum payload length in bytes.
#define UART_PROTO_PAYLOAD_MAX         (1024U)

// Header size: start_marker(1) + command(1) + len(2).
#define UART_PROTO_HEADER_SIZE         (4U)

// Tail size: crc16(2) + end_marker(1).
#define UART_PROTO_TAIL_SIZE           (3U)

// Maximum full on-wire frame size.
#define UART_PROTO_FRAME_MAX           (UART_PROTO_HEADER_SIZE + UART_PROTO_PAYLOAD_MAX + UART_PROTO_TAIL_SIZE)

// Field offsets within the frame.
#define UART_PROTO_OFF_START           (0U)
#define UART_PROTO_OFF_CMD             (1U)
#define UART_PROTO_OFF_LEN_L           (2U)
#define UART_PROTO_OFF_LEN_H           (3U)
#define UART_PROTO_OFF_PAYLOAD         (4U)



// PUBLIC TYPES

// Commands.
typedef enum
{
    UART_PROTO_CMD_NOP     = 0x00U,   // No operation (ping).
    UART_PROTO_CMD_PING    = 0x01U,   // Link check.
    UART_PROTO_CMD_ECHO    = 0x02U,   // Echo payload back.
    UART_PROTO_CMD_GET_VER = 0x03U,   // Get version request.
    UART_PROTO_CMD_RESET   = 0x04U,   // Reset.
    UART_PROTO_CMD_COUNT,             // Command count (not a command).
} uart_proto_cmd_t;

// Status codes.
typedef enum
{
    UART_PROTO_OK = 0,                // Success.
    UART_PROTO_ERR_NULL,              // NULL argument.
    UART_PROTO_ERR_NO_DATA,           // No data source.
    UART_PROTO_ERR_START,             // Bad start marker.
    UART_PROTO_ERR_LEN,               // Invalid length.
    UART_PROTO_ERR_CRC,               // CRC mismatch.
    UART_PROTO_ERR_END,               // Bad end marker.
    UART_PROTO_ERR_UNKNOWN_CMD,       // Unknown command.
} uart_proto_status_t;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // UART_PROTO_CMD_DEF_H

/** @}*/
