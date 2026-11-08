/**
  * @file      ring_buffer.c
  *
  * @brief     //TODO
  *
  *
  */

/** \addtogroup ring_buffer
*  @{
*/

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "ring_buffer.h"

#include <string.h>



/******************************************************************************
 * DEFINES
 ******************************************************************************/

// None.



/******************************************************************************
 * PRIVATE TYPES
 ******************************************************************************/

// None.



/******************************************************************************
 * PRIVATE DATA
 ******************************************************************************/

// None.



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

// None.



/******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

// None.



/******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

RING_BUF_INDEX_T RING_BUF_PushBlock(ring_buf_t *p_rb,
                                    const uint8_t *p_src,
                                    RING_BUF_INDEX_T len)
{
    RING_BUF_INDEX_T written = 0U;

    if ((p_rb != NULL) && (p_src != NULL) && (0U != len) &&
        (0U != p_rb->size))
    {
        RING_BUF_INDEX_T idx = 0U;

        for (idx = 0U; idx < len; idx++)
        {
            (void)RING_BUF_Push(p_rb, p_src[idx]);
            written++;
        }
    }

    return written;
}



RING_BUF_INDEX_T RING_BUF_PopBlock(ring_buf_t *p_rb,
                                   uint8_t *p_dst,
                                   RING_BUF_INDEX_T len)
{
    RING_BUF_INDEX_T read_cnt = 0U;

    if ((p_rb != NULL) && (p_dst != NULL) && (0U != len) &&
        (0U != p_rb->size))
    {
        RING_BUF_INDEX_T avail   = RING_BUF_Count(p_rb);
        RING_BUF_INDEX_T n       = 0U;
        RING_BUF_INDEX_T tail    = 0U;
        RING_BUF_INDEX_T first   = 0U;
        RING_BUF_INDEX_T chunk_a = 0U;

        n = (len < avail) ? len : avail;

        if (0U != n)
        {
            tail    = p_rb->tail;
            first   = p_rb->size - tail;
            chunk_a = (n < first) ? n : first;

            (void)memcpy(p_dst, &p_rb->p_buf[tail], chunk_a);

            if (n > chunk_a)
            {
                RING_BUF_INDEX_T rest = (RING_BUF_INDEX_T)(n - chunk_a);

                (void)memcpy(&p_dst[chunk_a], &p_rb->p_buf[0], rest);
            }

            RING_BUF_MEMORY_BARRIER();
            p_rb->tail = (RING_BUF_INDEX_T)((tail + n) % p_rb->size);
            p_rb->full = false;
            read_cnt   = n;
        }
    }

    return read_cnt;
}



void RING_BUF_Reset(ring_buf_t *p_rb)
{
    if (p_rb != NULL)
    {
        RING_BUF_MEMORY_BARRIER();
        p_rb->tail        = p_rb->head;
        p_rb->full        = false;
        p_rb->overwritten = false;
    }
}



/******************************************************************************
 * END OF SOURCE'S CODE
 ******************************************************************************/
/** @}*/
