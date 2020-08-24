
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "osal.h"
#include "SDK_EVAL_Config.h"
#include "Gatt_db.h"
#include "_NRF905_.h"

uint16_t chatServHandle, TXCharHandle, RXCharHandle;

/* UUIDs */
Service_UUID_t service_uuid;
Char_UUID_t char_uuid;

char get_mod[3];

extern uint8_t query_B1[];
extern uint8_t query_B2[];
extern uint8_t query_B3[];
extern uint8_t query_B4[];
extern uint8_t ch;
extern uint8_t addr[];

/*******************************************************************************
* Function Name  : Add_ChatService
* Description    : Add the 'Accelerometer' service.
* Input          : None
* Return         : Status.
*******************************************************************************/
uint8_t Add_Service(void)
{
  uint8_t ret;

  /*
  UUIDs:
  D973F2E0-B19E-11E2-9E96-0800200C9A66
  D973F2E1-B19E-11E2-9E96-0800200C9A66
  D973F2E2-B19E-11E2-9E96-0800200C9A66
  */

  const uint8_t uuid[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
  const uint8_t charUuidTX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
  const uint8_t charUuidRX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};

  Osal_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
  
  ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 6, &chatServHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;    

  Osal_MemCpy(&char_uuid.Char_UUID_128, charUuidTX, 16);
  ret =  aci_gatt_add_char(chatServHandle, UUID_TYPE_128, &char_uuid, 20, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
                16, 1, &TXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  Osal_MemCpy(&char_uuid.Char_UUID_128, charUuidRX, 16);
  ret =  aci_gatt_add_char(chatServHandle, UUID_TYPE_128, &char_uuid, 20, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                16, 1, &RXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  printf("Chat Service added.\nTX Char Handle %04X, RX Char Handle %04X\n", TXCharHandle, RXCharHandle);
  return BLE_STATUS_SUCCESS; 

fail:
  printf("Error while adding Chat service.\n");
  return BLE_STATUS_ERROR ;
}

/*******************************************************************************
* Function Name  : Attribute_Modified_CB
* Description    : Attribute modified callback.
* Input          : Attribute handle modified.
*                  Length of the data.
*                  Attribute data.
* Return         : None.
*******************************************************************************/
void Attribute_Modified_CB(uint16_t handle, uint16_t data_length, uint8_t *att_data)
{
	char get_dat[data_length]; 
  if(handle == RXCharHandle + 1)
  {
    for(int i = 0; i < data_length; i++)	
		get_dat[i] = att_data[i];
  }
		 for(int j = 0; j < 3; j++)
			{
				get_mod[j] = att_data[j];
			}
				
			if(strcmp(get_mod,"bt1") == 0)
			{ 
				int l = 0;
				memset(query_B1, 0, 9);
				for(int k = 3; k < data_length; k++)
				{
					query_B1[l] = att_data[k];
					l++;
				}
			} 
			
			if(strcmp(get_mod,"bt2") == 0)
				{ 
					int l = 0;
					memset(query_B2, 0, 9);
					for(int k = 3; k < data_length; k++)
					{
						query_B2[l] = att_data[k];
						l++;
					}
				} 
				
			if(strcmp(get_mod,"bt3") == 0)
			{ 
				int l = 0;
				memset(query_B3, 0, 9);
				for(int k = 3; k < data_length; k++)
				{
					query_B3[l] = att_data[k];
					l++;
				}
			} 
		
		if(strcmp(get_mod,"bt4") == 0)
			{ 
				int l = 0;
				memset(query_B4, 0, 9);
				for(int k = 3; k < data_length; k++)
				{
					query_B4[l] = att_data[k];
					l++;
				}
			} 
			
		if(strcmp(get_mod,"adr") == 0)
			{ 
				uint8_t adr[6];
				int l = 0;
				memset(addr, 0, 4); 
				for(int k = 3; k < data_length; k++)
				{
					adr[l] = att_data[k];
					l++;
				}
				
				int g =0;
				for(int y = 0; y < 4; y++)
				{
					addr[y] = (FromHex(adr[g])) << 4 | (FromHex(adr[g+1])); 
					g = g+2;
				}
				spiWriteRegister(0x05,addr[0]);spiWriteRegister(0x06,addr[1]);spiWriteRegister(0x07,addr[2]);spiWriteRegister(0x08,addr[3]);
			}
			
		if(strcmp(get_mod,"frq") == 0)
			{ 
				int l = 0; 
				uint8_t to_chan[5]; int ind; 
				uint8_t point = 46;float freq = 0.0;
				memset(to_chan, 0, sizeof(to_chan)); 
				for(int k = 3; k < data_length; k++)
				{
					to_chan[l] = att_data[k];
					l++;
				}
				
				int len = sizeof(to_chan);
				for (int p = 0; p < sizeof(to_chan); p++)
				{
						 if (to_chan[p] == point)
						 {
								 ind = p;
						 }
				}
				 int z = len - ind ;
				
				for(int c = 0 ; c < len ; c++)
				{
					if(c < ind)
					{
						freq = freq + ((int)(to_chan[c])%16) * pow(10,z); z = z -1 ;						
					}
					if(c > ind)
					{
						freq = freq + ((int)(to_chan[c])%16) * pow(10,z); z = z -1 ;						
					}
				}
				ch = ch_no(freq);
				setChannel(ch,FALSE);
			} 			
}

uint8_t FromHex(char c)
   {
   switch(c)
      {
      case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;
      case 'a': return 10;
      case 'b': return 11;
      case 'c': return 12;
      case 'd': return 13;
      case 'e': return 14;
      case 'f': return 15;
			case 'A': return 10;
      case 'B': return 11;
      case 'C': return 12;
      case 'D': return 13;
      case 'E': return 14;
      case 'F': return 15;
      }
		}