/**
  * @file      main.c
  *
  * @brief     Entry point
  *
  * @date      2023-01-20
  *
  */

/** \addtogroup main
*  @{
*/

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "app.h"

/******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/**
 * @brief Firmware entry point
 *
 * @warning This function shouldn't reach return. Inside this function there are infinite loop
 *
 * @return int always return 0
 */
int main (void)
{
    app_entry_point();

    return 0;
}

/******************************************************************************
 * END OF SOURCE'S CODE
 ******************************************************************************/
/** @}*/