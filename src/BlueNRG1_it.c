/**
  ******************************************************************************
  * @file    BlueNRG1_it.c 
  * @author  VMA RF Application Team
  * @version V1.0.0
  * @date    September-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "clock.h"
#include "SDK_EVAL_Button.h"
#include "stdio.h"
#include "_NRF905_.h"
#include "stdbool.h."

extern bool bt;
/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_Examples
  * @{
  */ 

/** @addtogroup GPIO_IOToggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles SVCall exception.
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  */
void SysTick_Handler(void)
{

}


void GPIO_Handler(void)
{
  /* If BUTTON_1 is pressed LED1 is ON */
		if(GPIO_GetITPendingBit(GPIO_Pin_9) == SET) 
			{
				if(GPIO_ReadBit(GPIO_Pin_9) == SET)
				{
					GPIO_ClearITPendingBit(GPIO_Pin_9);
					printf("IN IRQ 1\n");
				}
			}
		if(GPIO_GetITPendingBit(GPIO_Pin_10) == SET) 
			{
				if(GPIO_ReadBit(GPIO_Pin_10) == SET)
				{
					GPIO_ClearITPendingBit(GPIO_Pin_10);
					printf("IN IRQ 2\n");
				}
			}
    if(GPIO_GetITPendingBit(GPIO_Pin_12) == SET) 
			{
				if(GPIO_ReadBit(GPIO_Pin_12) == SET)
				{
					GPIO_ClearITPendingBit(GPIO_Pin_12);
					bt = true;
					printf("IN IRQ 3\n");
				}
			}
		if(GPIO_GetITPendingBit(GPIO_Pin_13) == SET) 
			{
			if(GPIO_ReadBit(GPIO_Pin_13) == SET)
				{
					GPIO_ClearITPendingBit(GPIO_Pin_13);
					printf("IN IRQ 4\n");
				}
			}
}

/******************************************************************************/
/*                 BlueNRG-1 Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (system_bluenrg1.c).                                               */
/******************************************************************************/
/**
* @brief  This function handles UART interrupt request.
* @param  None
* @retval None
*/
void UART_Handler(void)
{  
}

void Blue_Handler(void)
{
   // Call RAL_Isr
   RAL_Isr();
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/