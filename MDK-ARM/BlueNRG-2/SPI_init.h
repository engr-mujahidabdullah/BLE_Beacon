#ifndef __SPI_INIT_H__
#define  __SPI_INIT_H__

#include <stdio.h>
#include <stdint.h>

#define SPI_CS_MS_DEMO_PIN           GPIO_Pin_1

void SPI_Master_Configuration(void);
void spiWrite(uint8_t reg, uint8_t val);

uint8_t spiRead(uint8_t reg);
uint8_t spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len);
uint8_t spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len);
uint8_t spiCommand(uint8_t command);
uint8_t statusRead(void);
#endif
