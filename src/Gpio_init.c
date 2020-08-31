#include "Gpio_init.h"

#include "BlueNRG1_gpio.h"
#include <stdio.h>
#include <string.h>
#include "bluenrg_x_device.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"

void init_GPIO(void)
{

  
  /* Enable the GPIO Clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);
  
  /* Configure the LEDs */
  GPIO_InitType GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = Get_LedGpioPin(LED1) | Get_LedGpioPin(LED2);
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = ENABLE;
  GPIO_Init(&GPIO_InitStructure);

  /* Put the LEDs off */
  GPIO_WriteBit(Get_LedGpioPin(LED1), Bit_RESET);
  
  /* Configure the push buttons */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;// | GPIO_Pin_13;
	//GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Input;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  /* GPIO12 initialization during low power modes for BlueNRG-2. */
  GPIO_InitLowPowerModes(&GPIO_InitStructure);
  /* Set the output state of the GIO12 during low power modes. BlueNRG-2 only */
  GPIO_WriteBitsLowPowerModes(GPIO_Pin_12, Bit_SET);
  
//  /* Set the GPIO interrupt priority and enable it */
//  NVIC_InitType NVIC_InitStructure;
//  NVIC_InitStructure.NVIC_IRQChannel = GPIO_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//  /* Configures EXTI line for BUTTON_1 */
//		GPIO_EXTIConfigType GPIO_EXTIStructure;
//  GPIO_EXTIStructure.GPIO_Pin =  GPIO_Pin_9;
//  GPIO_EXTIStructure.GPIO_IrqSense = GPIO_IrqSense_Edge;
//  GPIO_EXTIStructure.GPIO_Event = IRQ_ON_RISING_EDGE;
//  GPIO_EXTIConfig(&GPIO_EXTIStructure);

//  /* Configures EXTI line for BUTTON_2 */
//  GPIO_EXTIStructure.GPIO_Pin =  GPIO_Pin_10;
//  GPIO_EXTIStructure.GPIO_Event = IRQ_ON_RISING_EDGE;
//  GPIO_EXTIConfig(&GPIO_EXTIStructure);
//	
//	 /* Configures EXTI line for BUTTON_1 */
//  GPIO_EXTIStructure.GPIO_Pin =  GPIO_Pin_11;
//  GPIO_EXTIStructure.GPIO_IrqSense = GPIO_IrqSense_Edge;
//  GPIO_EXTIStructure.GPIO_Event = IRQ_ON_RISING_EDGE;
//  GPIO_EXTIConfig(&GPIO_EXTIStructure);

//  /* Configures EXTI line for BUTTON_2 */
//  GPIO_EXTIStructure.GPIO_Pin =  GPIO_Pin_12;
//  GPIO_EXTIStructure.GPIO_Event = IRQ_ON_RISING_EDGE;
//  GPIO_EXTIConfig(&GPIO_EXTIStructure);
////  
//  /* Clear pending interrupt */
//  GPIO_ClearITPendingBit(GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13);
////  
////  /* Enable the interrupt */
//  GPIO_EXTICmd(GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13, ENABLE);
//	
	printf("Required GPIO PINS are Initialized\n");
}
