#ifndef __FLASH_WR_H__
#define  __FLASH_WR_H__
#include <stdio.h>
#include <stdint.h>

#define FLASH_ERASE_GUARD_TIME   25  // 25 ms
#define FLASH_WRITE_GUARD_TIME   1  // 1 ms
#define DATA_STORAGE_PAGE       (N_PAGES-8)
#define DATA_STORAGE_ADDR       (((N_PAGES-8)*N_BYTES_PAGE) + FLASH_START)

#define BT_MEM_1 1008
#define BT_MEM_2 1108
#define BT_MEM_3 1208
#define BT_MEM_4 1308

#define BT_CHN_W 1408
#define BT_ADR_W 1508

void write_flash(uint32_t addr, char message[], uint8_t size);						
uint8_t flash_read_addr(uint32_t addr);
void flash_read(uint32_t addr,uint8_t arry[], uint16_t size);

#endif
