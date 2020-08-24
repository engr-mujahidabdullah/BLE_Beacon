#ifndef __GATT_DB_H__
#define  __GATT_DB_H__

#include <stdio.h>
#include <stdint.h>


uint8_t FromHex(char c);
uint8_t Add_Service(void);
void Attribute_Modified_CB(uint16_t handle, uint16_t data_length, uint8_t *att_data);
#endif
