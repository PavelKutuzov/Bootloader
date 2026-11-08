/**
  * @file      ring_buffer.h
  *
  * @brief     //TODO
  *
  */

/** \addtogroup ring_buffer
*  @{
*/

#ifndef RNG_BUF_H
#define RNG_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "ring_buffer_cfg.h"



/******************************************************************************
 * DEFINES
 ******************************************************************************/

// None.



/******************************************************************************
 * PUBLIC TYPES
 ******************************************************************************/

// None.



/******************************************************************************
 * INLINE FUNCTIONS
 ******************************************************************************/

// Initialize the ring buffer descriptor.
static inline bool RING_BUF_Init(ring_buf_t *p_rb,
                                 uint8_t *p_mem,
                                 RING_BUF_INDEX_T size)
{
    bool ok = ((p_rb != NULL) && (p_mem != NULL) && (1U <= size));

    if (true == ok)
    {
        p_rb->p_buf       = p_mem;
        p_rb->size        = size;
        p_rb->head        = 0U;
        p_rb->tail        = 0U;
        p_rb->full        = false;
        p_rb->overwritten = false;
    }

    return ok;
}



// Check whether the buffer is empty.
static inline bool RING_BUF_IsEmpty(const ring_buf_t *p_rb)
{
    return ((p_rb == NULL) ||
            ((p_rb->head == p_rb->tail) && (false == p_rb->full)));
}



// Check whether the buffer is full.
static inline bool RING_BUF_IsFull(const ring_buf_t *p_rb)
{
    return ((p_rb != NULL) && (0U != p_rb->size) && (true == p_rb->full));
}



// Current number of elements in the buffer.
static inline RING_BUF_INDEX_T RING_BUF_Count(const ring_buf_t *p_rb)
{
    RING_BUF_INDEX_T count = 0U;

    if ((p_rb != NULL) && (0U != p_rb->size))
    {
        if (true == p_rb->full)
        {
            count = p_rb->size;
        }
        else
        {
            RING_BUF_INDEX_T head = p_rb->head;
            RING_BUF_INDEX_T tail = p_rb->tail;

            if (head >= tail)
            {
                count = (RING_BUF_INDEX_T)(head - tail);
            }
            else
            {
                count = (RING_BUF_INDEX_T)(p_rb->size - tail + head);
            }
        }
    }

    return count;
}



// Free space in the buffer (in elements).
static inline RING_BUF_INDEX_T RING_BUF_Free(const ring_buf_t *p_rb)
{
    RING_BUF_INDEX_T free_cnt = 0U;

    if ((p_rb != NULL) && (0U != p_rb->size))
    {
        free_cnt = (RING_BUF_INDEX_T)(p_rb->size - RING_BUF_Count(p_rb));
    }

    return free_cnt;
}



// Check and report the "data overwritten" flag.
static inline bool RING_BUF_IsOverwritten(const ring_buf_t *p_rb)
{
    return ((p_rb != NULL) && (true == p_rb->overwritten));
}

// Clear the "data overwritten" flag.
static inline void RING_BUF_ClearOverwriteFlag(ring_buf_t *p_rb)
{
    if (p_rb != NULL)
    {
        p_rb->overwritten = false;
    }
}



// Push one byte (producer side). Overwrites the oldest byte if full.
static inline bool RING_BUF_Push(ring_buf_t *p_rb,
                                 uint8_t byte)
{
    bool ok = ((p_rb != NULL) && (0U != p_rb->size));

    if (true == ok)
    {
        p_rb->p_buf[p_rb->head] = byte;

        if (true == p_rb->full)
        {
            p_rb->tail        = (RING_BUF_INDEX_T)((p_rb->tail + 1U) % p_rb->size);
            p_rb->overwritten = true;
        }

        RING_BUF_MEMORY_BARRIER();
        p_rb->head = (RING_BUF_INDEX_T)((p_rb->head + 1U) % p_rb->size);

        if (p_rb->head == p_rb->tail)
        {
            p_rb->full = true;
        }
    }

    return ok;
}



// Pop one byte (consumer side).
static inline bool RING_BUF_Pop(ring_buf_t *p_rb,
                                uint8_t *p_byte)
{
    bool popped = false;

    if ((p_rb != NULL) && (p_byte != NULL) && (0U != p_rb->size))
    {
        if ((true == p_rb->full) || (p_rb->tail != p_rb->head))
        {
            *p_byte = p_rb->p_buf[p_rb->tail];
            RING_BUF_MEMORY_BARRIER();
            p_rb->tail = (RING_BUF_INDEX_T)((p_rb->tail + 1U) % p_rb->size);
            p_rb->full = false;
            popped     = true;
        }
    }

    return popped;
}



/******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES
 ******************************************************************************/

// Push a block of bytes (producer side). Overwrite allowed.
RING_BUF_INDEX_T RING_BUF_PushBlock(ring_buf_t *p_rb,
                                    const uint8_t *p_src,
                                    RING_BUF_INDEX_T len);

// Pop a block of bytes (consumer side).
RING_BUF_INDEX_T RING_BUF_PopBlock(ring_buf_t *p_rb,
                                   uint8_t *p_dst,
                                   RING_BUF_INDEX_T len);

// Reset (empty) the buffer and clear the overwrite flag.
void RING_BUF_Reset(ring_buf_t *p_rb);



/******************************************************************************
 * END OF HEADER'S CODE
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // RNG_BUF_H

/** @}*/
