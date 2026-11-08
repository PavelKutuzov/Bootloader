/**
  * @file      uart_proto.c
  *
  * @brief     //TODO
  *
  */

/** \addtogroup uart_proto
*  @{
*/

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "uart_proto.h"

#include <string.h>



/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define UART_PROTO_CRC_POLY            (0xA001U)
#define UART_PROTO_CRC_INIT            (0xFFFFU)



/******************************************************************************
 * PRIVATE TYPES
 ******************************************************************************/

// Parsing FSM states.
typedef enum
{
    UART_PROTO_FSM_WAIT_START = 0, // Waiting for start_marker.
    UART_PROTO_FSM_CMD,            // Receiving command.
    UART_PROTO_FSM_LEN_L,          // Receiving len LSB.
    UART_PROTO_FSM_LEN_H,          // Receiving len MSB.
    UART_PROTO_FSM_PAYLOAD,        // Receiving payload.
    UART_PROTO_FSM_CRC_L,          // Receiving CRC LSB.
    UART_PROTO_FSM_CRC_H,          // Receiving CRC MSB.
    UART_PROTO_FSM_END,            // Receiving end_marker.
} uart_proto_fsm_t;


// Parser context.
typedef struct
{
    uart_proto_fsm_t state;                           // Current FSM state.
    uint16_t         payload_cnt;                     // Payload bytes received so far.
    uint16_t         len;                             // Expected payload length.
    uint16_t         crc_rx;                          // Received CRC (assembled bytewise).
    uint16_t         crc_calc;                        // Accumulated CRC.
    uint8_t          raw_hdr[UART_PROTO_HEADER_SIZE]; // Debug/CRC scratch.
} uart_proto_ctx_t;



/******************************************************************************
 * PRIVATE DATA
 ******************************************************************************/

static ring_buf_t *       s_p_ring = NULL; // Ring buffer (assigned externally).
static uart_proto_ctx_t   s_ctx;           // FSM context.
static uart_proto_frame_t s_frame_parsed;  // Last parsed frame.



/******************************************************************************
 * PUBLIC DATA
 ******************************************************************************/

// None.



/******************************************************************************
 * EXTERNAL DATA
 ******************************************************************************/

// None.



/******************************************************************************
 * EXTERNAL FUNCTION PROTOTYPES
 ******************************************************************************/

// None.



/******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/

static bool     UART_PROTO_PopRing(uint8_t *p_byte);

static uint16_t UART_PROTO_UpdateCrc16(uint16_t crc,
                                       uint8_t byte);

static void     UART_PROTO_ResetFsm(uart_proto_ctx_t *p_ctx);

static void     UART_PROTO_StepFsm(uart_proto_ctx_t *p_ctx,
                                   uint8_t byte,
                                   bool *p_done);



/******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

// Pop one byte from the ring buffer.
static bool UART_PROTO_PopRing(uint8_t *p_byte)
{
    return RING_BUF_Pop(s_p_ring, p_byte);
}



// Update CRC16 (Modbus, poly=0xA001, init=0xFFFF).
static uint16_t UART_PROTO_UpdateCrc16(uint16_t crc,
                                       uint8_t byte)
{
    uint16_t result = crc;
    uint8_t  idx    = 0U;

    result ^= (uint16_t)byte;

    for (idx = 0U; idx < 8U; idx++)
    {
        if (0U != (result & 0x0001U))
        {
            result = (uint16_t)((result >> 1U) ^ UART_PROTO_CRC_POLY);
        }
        else
        {
            result = (uint16_t)(result >> 1U);
        }
    }

    return result;
}



// Reset the FSM.
static void UART_PROTO_ResetFsm(uart_proto_ctx_t *p_ctx)
{
    if (p_ctx != NULL)
    {
        p_ctx->state       = UART_PROTO_FSM_WAIT_START;
        p_ctx->payload_cnt = 0U;
        p_ctx->len         = 0U;
        p_ctx->crc_rx      = 0U;
        p_ctx->crc_calc    = UART_PROTO_CRC_INIT;
        (void)memset(p_ctx->raw_hdr, 0, sizeof(p_ctx->raw_hdr));
    }
}



// One FSM step for a received byte.
static void UART_PROTO_StepFsm(uart_proto_ctx_t *p_ctx,
                               uint8_t byte,
                               bool *p_done)
{
    if ((p_ctx != NULL) && (p_done != NULL))
    {
        *p_done = false;

        switch (p_ctx->state)
        {
            case UART_PROTO_FSM_WAIT_START:
            {
                if (UART_PROTO_START_MARKER == byte)
                {
                    p_ctx->crc_calc = UART_PROTO_CRC_INIT;
                    p_ctx->crc_calc = UART_PROTO_UpdateCrc16(p_ctx->crc_calc, byte);
                    p_ctx->state    = UART_PROTO_FSM_CMD;
                }

                break;
            }

            case UART_PROTO_FSM_CMD:
            {
                s_frame_parsed.command = byte;
                p_ctx->crc_calc        = UART_PROTO_UpdateCrc16(p_ctx->crc_calc, byte);
                p_ctx->state           = UART_PROTO_FSM_LEN_L;
                break;
            }

            case UART_PROTO_FSM_LEN_L:
            {
                p_ctx->len      = (uint16_t)byte;
                p_ctx->crc_calc = UART_PROTO_UpdateCrc16(p_ctx->crc_calc, byte);
                p_ctx->state    = UART_PROTO_FSM_LEN_H;
                break;
            }

            case UART_PROTO_FSM_LEN_H:
            {
                p_ctx->len     |= (uint16_t)((uint16_t)byte << 8U);
                p_ctx->crc_calc = UART_PROTO_UpdateCrc16(p_ctx->crc_calc, byte);

                if (UART_PROTO_PAYLOAD_MAX < p_ctx->len)
                {
                    UART_PROTO_ResetFsm(p_ctx);
                }
                else
                {
                    p_ctx->payload_cnt = 0U;

                    if (0U == p_ctx->len)
                    {
                        p_ctx->state = UART_PROTO_FSM_CRC_L;
                    }
                    else
                    {
                        p_ctx->state = UART_PROTO_FSM_PAYLOAD;
                    }
                }

                break;
            }

            case UART_PROTO_FSM_PAYLOAD:
            {
                s_frame_parsed.payload[p_ctx->payload_cnt] = byte;
                p_ctx->payload_cnt++;
                p_ctx->crc_calc = UART_PROTO_UpdateCrc16(p_ctx->crc_calc, byte);

                if (p_ctx->payload_cnt >= p_ctx->len)
                {
                    p_ctx->state = UART_PROTO_FSM_CRC_L;
                }

                break;
            }

            case UART_PROTO_FSM_CRC_L:
            {
                p_ctx->crc_rx = (uint16_t)byte;
                p_ctx->state  = UART_PROTO_FSM_CRC_H;
                break;
            }

            case UART_PROTO_FSM_CRC_H:
            {
                p_ctx->crc_rx |= (uint16_t)((uint16_t)byte << 8U);
                p_ctx->state   = UART_PROTO_FSM_END;
                break;
            }

            case UART_PROTO_FSM_END:
            {
                if ((UART_PROTO_END_MARKER == byte) &&
                    (p_ctx->crc_rx == p_ctx->crc_calc))
                {
                    s_frame_parsed.len = p_ctx->len;
                    s_frame_parsed.crc = p_ctx->crc_rx;
                    *p_done            = true;
                }

                UART_PROTO_ResetFsm(p_ctx);
                break;
            }

            default:
            {
                // Unknown state; resynchronize.
                UART_PROTO_ResetFsm(p_ctx);
                break;
            }
        }
    }
}



/******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

uart_proto_status_t UART_PROTO_SetRingBuffer(ring_buf_t *p_ring)
{
    uart_proto_status_t status = UART_PROTO_OK;

    if ((p_ring == NULL) || (p_ring->p_buf == NULL) || (0U == p_ring->size))
    {
        status = UART_PROTO_ERR_NULL;
    }
    else
    {
        s_p_ring = p_ring;
        RING_BUF_Reset(p_ring);
        UART_PROTO_ResetFsm(&s_ctx);
    }

    return status;
}



uart_proto_status_t UART_PROTO_ParsePoll(bool *const p_received)
{
    uart_proto_status_t status = UART_PROTO_OK;

    if (p_received == NULL)
    {
        status = UART_PROTO_ERR_NULL;
    }
    else
    {
        *p_received = false;

        if ((s_p_ring == NULL) || (s_p_ring->p_buf == NULL))
        {
            status = UART_PROTO_ERR_NO_DATA;
        }
        else
        {
            uint8_t byte = 0U;
            bool    done = false;

            while ((false == done) && (true == UART_PROTO_PopRing(&byte)))
            {
                UART_PROTO_StepFsm(&s_ctx, byte, &done);
            }

            if (true == done)
            {
                *p_received = true;
            }
        }
    }

    return status;
}



const uart_proto_frame_t *UART_PROTO_GetParsedFrame(void)
{
    return &s_frame_parsed;
}



void UART_PROTO_Reset(void)
{
    UART_PROTO_ResetFsm(&s_ctx);
}



uint16_t UART_PROTO_CalcCrc16(const uint8_t *p_data,
                              uint16_t len)
{
    uint16_t crc = UART_PROTO_CRC_INIT;

    if (p_data != NULL)
    {
        uint16_t idx = 0U;

        for (idx = 0U; idx < len; idx++)
        {
            crc = UART_PROTO_UpdateCrc16(crc, p_data[idx]);
        }
    }

    return crc;
}



uart_proto_status_t UART_PROTO_BuildFrame(uint8_t cmd,
                                          const uint8_t *p_payload,
                                          uint16_t len,
                                          uint8_t *p_out,
                                          uint16_t out_size,
                                          uint16_t *p_out_len)
{
    uart_proto_status_t status = UART_PROTO_OK;
    uint16_t need = 0U;

    if ((p_out == NULL) || (p_out_len == NULL) ||
        ((p_payload == NULL) && (0U != len)))
    {
        status = UART_PROTO_ERR_NULL;
    }
    else if (UART_PROTO_PAYLOAD_MAX < len)
    {
        status = UART_PROTO_ERR_LEN;
    }
    else
    {
        need = (uint16_t)(UART_PROTO_HEADER_SIZE + len + UART_PROTO_TAIL_SIZE);

        if (out_size < need)
        {
            status = UART_PROTO_ERR_LEN;
        }
    }

    if (UART_PROTO_OK == status)
    {
        uint16_t idx = 0U;
        uint16_t crc = 0U;

        p_out[idx] = UART_PROTO_START_MARKER;
        idx++;
        p_out[idx] = cmd;
        idx++;
        p_out[idx] = (uint8_t)(len & 0xFFU);
        idx++;
        p_out[idx] = (uint8_t)((len >> 8U) & 0xFFU);
        idx++;

        if (0U != len)
        {
            (void)memcpy(&p_out[idx], p_payload, len);
            idx = (uint16_t)(idx + len);
        }

        crc = UART_PROTO_CalcCrc16(p_out, idx);
        p_out[idx] = (uint8_t)(crc & 0xFFU);
        idx++;
        p_out[idx] = (uint8_t)((crc >> 8U) & 0xFFU);
        idx++;
        p_out[idx] = UART_PROTO_END_MARKER;
        idx++;

        *p_out_len = idx;
    }

    return status;
}



/******************************************************************************
 * END OF SOURCE'S CODE
 ******************************************************************************/
/** @}*/
