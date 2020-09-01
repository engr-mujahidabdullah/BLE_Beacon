/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "string.h"
#include "bluenrg_x_device.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "SDK_EVAL_SPI.h"
#include "ble_const.h"

#include "clock.h"
#include "sleep.h"
#include "Beacon_config.h"
#include "OTA_btl.h"
#include "osal.h"
#include "BlueNRG1_spi.h"

#include "Gpio_init.h"
#include "ble_init.h"
#include "_NRF905_.h"
#include "Gatt_db.h"
#include "flash_wr.h"
#include "stdbool.h."
#include "time.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_BEACON_VERSION_STRING "1.1.0"

/* Set to 1 for enabling Flags AD Type position at the beginning 
   of the advertising packet */
#define ENABLE_FLAGS_AD_TYPE_AT_BEGINNING 1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t Next_Advertising_SysTime = 0;
// send data associated to buttons
uint8_t query_B1[] = "button_1";
uint8_t query_B2[] = "button_2";
uint8_t query_B3[] = "button_3";
uint8_t query_B4[] = "button_4";
uint8_t greatings[] = "HELLO SIR";

uint8_t addr[] = {0xE7,0xE7,0xE7,0xE7}; //Default Address of NRF-905
uint8_t ch = 108;	// Default Chanel of NRF-905

// limit variabloes
bool bt = false;
bool check = false;
bool pinCheck = true;

/* Private function prototypes -----------------------------------------------*/
void Device_Init(void);  						// Device initialization function
static void Start_Beaconing(void); //Beconing start function
void flash_check(void); 					// check data from flash memory
void flash_data_read(void); 		 //Function to read data from memory
	
/*----------------------------------------------------------------------------*/
/* Enable the Standby - SLEEPMODE_NOTIMER */
void sleep(void)
{
  uint8_t ret, wakeup_source, wakeup_level;

  wakeup_source = WAKEUP_IO12;
  wakeup_level = (WAKEUP_IOx_HIGH << WAKEUP_IO12_SHIFT_MASK);

  while(SdkEvalComUARTBusy() == SET);
  ret = BlueNRG_Sleep(SLEEPMODE_NOTIMER, wakeup_source, wakeup_level);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("BlueNRG_Sleep() error 0x%02x\r\n", ret);
    while(1);
  }
}

int main(void) 
{
  uint8_t ret;
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG-1 platform */
  SdkEvalIdentification();

  /* Init the UART peripheral */
  SdkEvalComUartInit(UART_BAUDRATE); 
	
	//while(1);
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) 
	{
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  /* Init the BlueNRG-1 device */
  Device_Init();

	/* Check the memory either empty or not and write flash memory to default if needed*/
	flash_check();

	/* read existed wariables from memory on setup */
	flash_data_read();

	/* Initialize NRF-905 & data blocks*/
	nrf_init(); 
	tran_int();	
	
	/* Initiallize Gpio*/
	init_GPIO();
	
	/* Add Services */
	Add_Service();
	printRegisters();
	/* Enable the callback end_of_radio_activity_event for advertising */
  aci_hal_set_radio_activity_mask(0x0002); 
	
#if ST_USE_OTA_SERVICE_MANAGER_APPLICATION
  /* Initialize the button */
  SdkEvalPushButtonInit(USER_BUTTON); 
#endif /* ST_USE_OTA_SERVICE_MANAGER_APPLICATION */
	
	/* Send greatings on Setup */
	send_data(greatings, 9);
	
	for(int c = 0; c < 10 ; c++);
	uint8_t buffer[RH_NRF905_MAX_MESSAGE_LEN];
   uint8_t length = sizeof(buffer);
	GPIO_WriteBit( _txEnablePin, Bit_RESET);
	GPIO_WriteBit( _chipEnablePin, Bit_SET);
	//uint8_t status = statusRead();
	//printf("%d", status & RH_NRF905_STATUS_DR);
	//Clock_Init();
	while(1)
	{
		data_recv(buffer, &length);
		//Clock_Wait(1000);
	}
	
  /* Start Beacon Non Connectable Mode*/
  Start_Beaconing();
  
  printf("BlueNRG-1 BLE Beacon Application (version: %s)\r\n", BLE_BEACON_VERSION_STRING); 
	

  GPIO_WriteBit(Get_LedGpioPin(LED1), LED_ON);
	
  while(1) 
  {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
		sleep();

		
		if(BlueNRG_WakeupSource() == WAKEUP_IO12)
		{
			if(check == true)
			send_data(query_B3, 9);
		}
		
		
#if ST_USE_OTA_SERVICE_MANAGER_APPLICATION
    if (SdkEvalPushButtonGetState(USER_BUTTON) == RESET)
    {
      OTA_Jump_To_Service_Manager_Application();
    }
#endif /* ST_USE_OTA_SERVICE_MANAGER_APPLICATION */
  }
}
/* Private functions ---------------------------------------------------------*/
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
/**
* @brief  Start beaconing
* @param  None 
* @retval None
*/
static void Start_Beaconing(void)
{  
  uint8_t ret = BLE_STATUS_SUCCESS;

#if ENABLE_FLAGS_AD_TYPE_AT_BEGINNING
  /* Set AD Type Flags at beginning on Advertising packet  */
  uint8_t adv_data[] = {
      /* Advertising data: Flags AD Type */
      0x02, 
      0x01, 
      0x06, 
      /* Advertising data: manufacturer specific data */
      26, //len
      AD_TYPE_MANUFACTURER_SPECIFIC_DATA,  //manufacturer type
      0x30, 0x00, //Company identifier code (Default is 0x0030 - STMicroelectronics: To be customized for specific identifier)
      0x02,       // ID
      0x15,       //Length of the remaining payload
      0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
      0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
      0x00, 0x00, // Major number 
      0x00, 0x00, // Minor number 
      0xC8        //2's complement of the Tx power (-56dB)};      
   };

#endif
   
  /* disable scan response */
  ret = hci_le_set_scan_response_data(0,NULL);
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in hci_le_set_scan_resp_data() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("hci_le_set_scan_resp_data() --> SUCCESS\r\n");

  /* put device in non connectable mode */
  ret = aci_gap_set_discoverable(ADV_IND , 160, 160, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 0, NULL, 0, NULL, 0, 0); 
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in aci_gap_set_discoverable() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("aci_gap_set_discoverable() --> SUCCESS\r\n");

#if ENABLE_FLAGS_AD_TYPE_AT_BEGINNING
  /* Set the  ADV data with the Flags AD Type at beginning of the 
     advertsing packet,  followed by the beacon manufacturer specific data */
  ret = hci_le_set_advertising_data (sizeof(adv_data), adv_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in hci_le_set_advertising_data() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("hci_le_set_advertising_data() --> SUCCESS\r\n");
#else
  /* Delete the TX power level information */
  ret = aci_gap_delete_ad_type(AD_TYPE_TX_POWER_LEVEL); 
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in aci_gap_delete_ad_type() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("aci_gap_delete_ad_type() --> SUCCESS\r\n");
#endif
}

void flash_check(void)
{
	uint8_t QB1 = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_MEM_1 +(0*4));
	uint8_t QB2 = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_MEM_2 +(0*4));
	uint8_t QB3 = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_MEM_3 +(0*4));
	uint8_t QB4 = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_MEM_4 +(0*4));
	uint8_t CHN = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_CHN_W +(0*4));
	uint8_t AD1 = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_ADR_W +(0*4));
	
	if(QB1 == 0xFF) write_flash(DATA_STORAGE_ADDR + (BT_MEM_1), (char*)query_B1, sizeof(query_B1));
	if(QB2 == 0xFF)	write_flash(DATA_STORAGE_ADDR + (BT_MEM_2),(char*) query_B2, sizeof(query_B2));
	if(QB3 == 0xFF)	write_flash(DATA_STORAGE_ADDR + (BT_MEM_3),(char*)query_B3, sizeof(query_B3));
	if(QB4 == 0xFF)	write_flash(DATA_STORAGE_ADDR + (BT_MEM_4),(char*)query_B4, sizeof(query_B4));
	if(CHN == 0xFF) FLASH_ProgramWord(DATA_STORAGE_ADDR + (BT_CHN_W), ch);
	if(AD1 == 0xFF)	write_flash(DATA_STORAGE_ADDR + (BT_ADR_W),addr, sizeof(addr));
}

void flash_data_read(void)
{
	for(int i = 0; i < sizeof(query_B1); i++)
		{
			query_B1[i] = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_MEM_1 +(i*4));
		}
		
		for(int i = 0; i < sizeof(query_B2); i++)
		{
			query_B2[i] = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_MEM_2 +(i*4));
		}
		
		for(int i = 0; i < sizeof(query_B3); i++)
		{
			query_B3[i] = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_MEM_3 +(i*4));
		}
		
		for(int i = 0; i < sizeof(query_B4); i++)
		{
			query_B4[i] = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_MEM_4 +(i*4));
		}
		
		for(int i = 0; i < sizeof(addr); i++)
		{
			addr[i] = FLASH_ReadWord(DATA_STORAGE_ADDR + BT_ADR_W +(i*4));
		}
		
		ch = FLASH_ReadByte(DATA_STORAGE_ADDR + BT_CHN_W );
		
}	

/*-------------------------------------------------------*/

/*-------------------------------------------------*/

/* Hardware Error event. 
   This event is used to notify the Host that a hardware failure has occurred in the Controller. 
   Hardware_Code Values:
   - 0x01: Radio state error
   - 0x02: Timer overrun error
   - 0x03: Internal queue overflow error
   After this event is recommended to force device reset. */

void hci_hardware_error_event(uint8_t Hardware_Code)
{
   NVIC_SystemReset();
}


/****************** BlueNRG-1 Sleep Management Callback ********************************/

SleepModes App_SleepMode_Check(SleepModes sleepMode)
{
  if(SdkEvalComIOTxFifoNotEmpty() || SdkEvalComUARTBusy())
    return SLEEPMODE_RUNNING;
  
  return SLEEPMODE_NOTIMER;
}

/***************************************************************************************/
void aci_hal_end_of_radio_activity_event(uint8_t Last_State,
                                         uint8_t Next_State,
                                         uint32_t Next_State_SysTime)
{
  if (Next_State == 0x01) /* 0x01: Advertising */
  {  /* Check if data for flash write are ready: do write only if there is enough time before next radio activity */
    Next_Advertising_SysTime = Next_State_SysTime;
  }
}
/*-----------------------------------------------------------*/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
	hci_le_set_advertise_enable(0x01);
	/* Erase the DATA_STORAGE_PAGE before write operation */
  FLASH_ErasePage(DATA_STORAGE_PAGE);
		
		write_flash(DATA_STORAGE_ADDR + (BT_MEM_1),(char*)query_B1, sizeof(query_B1));
		write_flash(DATA_STORAGE_ADDR + (BT_MEM_2),(char*)query_B2, sizeof(query_B2));
		write_flash(DATA_STORAGE_ADDR + (BT_MEM_3),(char*)query_B3, sizeof(query_B3));
		write_flash(DATA_STORAGE_ADDR + (BT_MEM_4),(char*)query_B4, sizeof(query_B4));
		FLASH_ProgramWord(DATA_STORAGE_ADDR + (BT_CHN_W), ch);
		write_flash(DATA_STORAGE_ADDR + (BT_ADR_W),addr, sizeof(addr));
}
/*******************************************************************************
 * Function Name  : aci_gatt_attribute_modified_event.
 * Description    : This event occurs when an attribute is modified.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,

                                       uint8_t Attr_Data[])
{ 
  Attribute_Modified_CB(Attr_Handle, Attr_Data_Length, Attr_Data); 
}
/***************************************************************************************/
#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}


#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
 */
