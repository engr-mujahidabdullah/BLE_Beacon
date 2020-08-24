

#include <stdio.h>
#include <string.h>
#include "bluenrg_x_device.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "clock.h"
#include "sleep.h"
#include "Beacon_config.h"
#include "OTA_btl.h"
#include "osal.h"
#include "ble_init.h"

void Device_Init(void)
{
  uint8_t ret;
  uint16_t service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  
  /* Set the TX Power to -2 dBm */
  ret = aci_hal_set_tx_power_level(1,4);
  if(ret != 0) {
    printf ("Error in aci_hal_set_tx_power_level() 0x%04xr\n", ret);
    while(1);
  }

  /* Init the GATT */
  ret = aci_gatt_init();
  if (ret != 0) 
    printf ("Error in aci_gatt_init() 0x%04xr\n", ret);
  else
    printf ("aci_gatt_init() --> SUCCESS\r\n");
  
  /* Init the GAP */
  ret = aci_gap_init(0x01, 0x00, 0x08, &service_handle, 
                     &dev_name_char_handle, &appearance_char_handle);
  if (ret != 0)
    printf ("Error in aci_gap_init() 0x%04x\r\n", ret);
  else
    printf ("aci_gap_init() --> SUCCESS\r\n");
	
}

