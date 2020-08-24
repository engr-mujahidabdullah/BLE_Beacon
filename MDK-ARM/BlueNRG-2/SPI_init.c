#include "SPI_init.h"

/* Includes ------------------------------------------------------------------*/
#include "bluenrg_x_device.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"

#include "BlueNRG1_gpio.h"
#include "BlueNRG1_spi.h"

/**
  * @brief  SPI Master initialization.
  * @param  None
  * @retval None
  */
void SPI_Master_Configuration(void)
{
  SPI_InitType SPI_InitStructure;
  GPIO_InitType GPIO_InitStructure;
  
  /* Enable SPI and GPIO clocks */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO | CLOCK_PERIPH_SPI, ENABLE);   
  
  /* Configure SPI pins */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_SPI_PERIPH_OUT_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_SPI_PERIPH_OUT_MODE;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_SPI_PERIPH_IN_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_SPI_PERIPH_IN_MODE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_SPI_PERIPH_SCLK_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_SPI_PERIPH_SCLK_MODE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI_CS_MS_DEMO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_HighPwr = ENABLE;
  GPIO_Init(&GPIO_InitStructure);
  GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
  
  /* Configure SPI in master mode */
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b ;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_BaudRate = SPI_BAUDRATE;
  SPI_Init(&SPI_InitStructure);
  
  /* Clear RX and TX FIFO */
  SPI_ClearTXFIFO();
  SPI_ClearRXFIFO();
  
  /* Set null character */
  SPI_SetDummyCharacter(0xFF);
  
  /* Set communication mode */
  SPI_SetMasterCommunicationMode(SPI_FULL_DUPLEX_MODE);
  
  /* Enable SPI functionality */
  SPI_Cmd(ENABLE);  

	printf("SPI INITIALIZED in MAster Mode\n");
}

void spiWrite(uint8_t reg, uint8_t val)
{
	/* Set communication mode */
  SPI_SetMasterCommunicationMode(SPI_TRANSMIT_MODE);
	
	GPIO_ResetBits(SPI_CS_MS_DEMO_PIN);
	
	/* Write command to be send send to TX FIFO */
	while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE)){};
  SPI_SendData(reg);
	while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE)){};
  SPI_SendData(val);
	
  /* Wait until data transfer is finished */
  while (SET == SPI_GetFlagStatus(SPI_FLAG_BSY));
	GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
}


uint8_t spiRead(uint8_t reg)
{
    		/* Set communication mode */
		SPI_SetMasterCommunicationMode(SPI_FULL_DUPLEX_MODE);
	
    uint8_t val;
		GPIO_ResetBits(SPI_CS_MS_DEMO_PIN);
	
		/* Write data to send to TX FIFO */
		while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE));
		SPI_SendData(reg);  
	
		while(RESET == SPI_GetFlagStatus(SPI_FLAG_RNE));
		SPI_ReceiveData();
	
		for(uint8_t i = 0; i< 1; i++) 
		{
    while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE));
    SPI_SendData(0x00);  
    while(RESET == SPI_GetFlagStatus(SPI_FLAG_RNE));
    val = SPI_ReceiveData();
		}
		
		while (SET == SPI_GetFlagStatus(SPI_FLAG_BSY)){};
		GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
    return val;
}

uint8_t spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len)
{
	/* Set communication mode */
		SPI_SetMasterCommunicationMode(SPI_FULL_DUPLEX_MODE);
	
		GPIO_ResetBits(SPI_CS_MS_DEMO_PIN);
	
    /* Write data to send to TX FIFO */
		while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE));
		SPI_SendData(reg); 
		while(RESET == SPI_GetFlagStatus(SPI_FLAG_RNE));
		SPI_ReceiveData();
    
		while (len--)
		{
			while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE));
			SPI_SendData(0x00);  
			while(RESET == SPI_GetFlagStatus(SPI_FLAG_RNE)){};
			*dest++ = SPI_ReceiveData();
		}
		while (SET == SPI_GetFlagStatus(SPI_FLAG_BSY)){};
		GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
			
    return TRUE;
}

uint8_t spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len)
{
    
    GPIO_ResetBits(SPI_CS_MS_DEMO_PIN);
			while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE)){};
	    SPI_SendData(reg); // Send the address
	
    while (len--)
		{
			while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE));
			SPI_SendData(*src++);
		}
		while (SET == SPI_GetFlagStatus(SPI_FLAG_BSY));
		GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
		
    return TRUE;
}


// Low level commands for interfacing with the device
uint8_t spiCommand(uint8_t command)
{
    uint8_t status;
    /* Set communication mode */
		SPI_SetMasterCommunicationMode(SPI_FULL_DUPLEX_MODE);
	
		GPIO_ResetBits(SPI_CS_MS_DEMO_PIN);
	
#if (RH_PLATFORM == RH_PLATFORM_MONGOOSE_OS)
    while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE)){};
	   SPI_SendData(command);
#else
    /* Write command to be send send to TX FIFO */
		while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE)){};
		SPI_SendData(command);
    while (SET == SPI_GetFlagStatus(SPI_FLAG_BSY));
		GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
#endif
    
    return status;
}

uint8_t statusRead(void)
{
    // The status is a byproduct of sending a command
    return spiCommand(0);
}

