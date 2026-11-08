/**
  * @file      bsp.c
  *
  * @brief     Some module
  *
  * @date      2022-12-21
  *
  */

/** \addtogroup module
*  @{
*/

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "bsp.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "uart.h"

#ifdef STM32F10X_MD_VL
#include "stm32f10x.h"
#elif GD32F30X_HD
#include "gd32f30x.h"
#endif


/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define BSP_SYSTICK_COMPARE_VALUE_1_MS                  (9000U)
#define BSP_NVIC_ICER_REGS_QTY                          (8U)



/******************************************************************************
 * PRIVATE TYPES
 ******************************************************************************/

// None.



/******************************************************************************
 * PRIVATE DATA
 ******************************************************************************/

static uint64_t bsp_current_time_ms = 0U;



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

static void BSP_ClockEnable(void);
static void BSP_GPIOInit(void);



/******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

static void BSP_ClockEnable(void)
{
    #ifdef STM32F10X_MD_VL

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // 72 MHz clock

    if (USART3 == BSP_UART_BOOT)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    }
    else if (USART1 == BSP_UART_BOOT)
    {
        // TODO
    }
    else
    {
        // Do nothing
    }
    #elif GD32F30X_HD
    // enable GPIO clock
    rcu_periph_clock_enable(RCU_GPIOA);

    // enable USART clock
    rcu_periph_clock_enable(RCU_USART0);
    #endif
}



static void BSP_GPIOInit(void)
{
    #ifdef STM32F10X_MD_VL
    GPIO_InitTypeDef GPIO_InitStruct;

	// Uart3 tx
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Uart3 rx
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    #elif GD32F30X_HD
    // connect port to USART0_Tx
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    // connect port to USART0_Rx
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    #endif
}



/******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

void BSP_Init(void)
{
    // Boot use IRQ, and before disable all IRQ channels then enable IRQ global, because the IRQ could be disabled globally before jump to boot.
    for (uint8_t i = 0; i < BSP_NVIC_ICER_REGS_QTY; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
    }

    __enable_irq();

    BSP_ClockEnable();
    BSP_GPIOInit();
    UART_Init(BSP_UART_BOOT);

	SysTick_Config(BSP_SYSTICK_COMPARE_VALUE_1_MS);

    #ifdef STM32F10X_MD_VL
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    #elif GD32F30X_HD
    systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);
    #endif
}

void SysTick_Handler(void)
{
	bsp_current_time_ms++;
}

uint64_t bsp_get_current_time(void)
{
	return bsp_current_time_ms;
}



/******************************************************************************
 * END OF SOURCE'S CODE
 ******************************************************************************/
/** @}*/