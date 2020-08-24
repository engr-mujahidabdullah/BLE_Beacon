#include "flash_wr.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "sleep.h"
#include "SDK_EVAL_Config.h"

void write_flash(uint32_t addr, char message[], uint8_t size)
{
  uint8_t message_length = size;
	printf("msg leng is %d: \n",message_length);
  uint8_t number_of_words = ((message_length / N_BYTES_WORD) + 1) * N_BYTES_WORD;
	/* Erase the DATA_STORAGE_PAGE before write operation */
  //FLASH_ErasePage(DATA_STORAGE_PAGE);
  
  /* Wait for the end of erase operation */
  while(FLASH_GetFlagStatus(Flash_CMDDONE) != SET);
    
  /* Write and verify operation */ 
  for(int i = 0; i < size ; i++)
		{
			/* Program the word */
				FLASH_ProgramWord(addr + i*4, message[i]);
				while(FLASH_GetFlagStatus(Flash_CMDDONE) != SET){;};
							printf("Write at address: 0x%08X is %c \n", addr + i*4, message[i]);
		}
	/* Wait for the end of write operation */
	printf("Flash Write Success\n");
}


uint8_t flash_read_addr(uint32_t addr)
{

	uint8_t memory_wor = FLASH_ReadWord(addr);
	printf("flash read at %x : %d\n",addr, memory_wor);
	return memory_wor;
}


void flash_read(uint32_t addr,uint8_t arry[], uint16_t size)
{
	printf("%d\n", size);
	for(int i = 0; i < size; i++)
	{
		arry[1] = (uint8_t)FLASH_ReadWord(addr + (1*4));
		printf("flash read at %x : %d\n",addr + (1*4), arry[1]);
	}	
}
