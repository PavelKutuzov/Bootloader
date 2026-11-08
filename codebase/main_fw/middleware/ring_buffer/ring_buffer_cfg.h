/**
  * @file      ring_buffer_cfg.h
  *
  * @brief     //TODO
  *
  */

/** \addtogroup ring_buffer_cfg
*  @{
*/

#ifndef RNG_BUF_CFG_H
#define RNG_BUF_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>



/******************************************************************************
 * DEFINES
 ******************************************************************************/

// Index/counter type of the ring buffer. Override to widen.
#ifndef RING_BUF_INDEX_T
#define RING_BUF_INDEX_T               uint16_t
#endif // RING_BUF_INDEX_T

// Memory barrier between producer and consumer.
#ifndef RING_BUF_MEMORY_BARRIER
#define RING_BUF_MEMORY_BARRIER()      do { } while (0)
#endif // RING_BUF_MEMORY_BARRIER



/******************************************************************************
 * PUBLIC TYPES
 ******************************************************************************/

// Ring buffer descriptor (SPSC, all slots usable, overwrite allowed).
typedef struct
{
    uint8_t *                 p_buf;       // Pointer to the buffer memory.
    RING_BUF_INDEX_T          size;        // Buffer size in elements.
    volatile RING_BUF_INDEX_T head;        // Write index (producer).
    volatile RING_BUF_INDEX_T tail;        // Read index (consumer).
    volatile bool             full;        // True if head == tail but not empty.
    volatile bool             overwritten; // True if any data was overwritten.
} ring_buf_t;



/******************************************************************************
 * INLINE FUNCTIONS
 ******************************************************************************/

// None.



/******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES
 ******************************************************************************/

// None.



/******************************************************************************
 * END OF HEADER'S CODE
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // RNG_BUF_CFG_H

/** @}*/
