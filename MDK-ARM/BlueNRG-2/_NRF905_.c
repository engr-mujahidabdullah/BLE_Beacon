#include "_NRF905_.h"
#include "stdint.h"
#include "osal.h"
#include "string.h"
#include "stdbool.h."

extern uint8_t ch;
extern uint8_t addr[];
    /// The receiver/transmitter buffer
    uint8_t             _buf[RH_NRF905_MAX_PAYLOAD_LEN];
		    /// TO header in the last received mesasge
    volatile uint8_t    _rxHeaderTo;

    /// FROM header in the last received mesasge
    volatile uint8_t    _rxHeaderFrom;

    /// ID header in the last received mesasge
    volatile uint8_t    _rxHeaderId;

    /// FLAGS header in the last received mesasge
    volatile uint8_t    _rxHeaderFlags;

    /// TO header to send in all messages
    uint8_t             _txHeaderTo;

    /// FROM header to send in all messages
    uint8_t             _txHeaderFrom;

    /// ID header to send in all messages
    uint8_t             _txHeaderId;
		
		/// FLAGS header to send in all messages
    uint8_t             _txHeaderFlags;
		
		    /// Count of the number of bad messages (eg bad checksum etc) received
    volatile uint16_t   _rxBad;

    /// Count of the number of successfully transmitted messaged
    volatile uint16_t   _rxGood;

    /// Count of the number of bad messages (correct checksum etc) received
    volatile uint16_t   _txGood;
		/// True when there is a valid message in the buffer
    BOOL                _rxBufValid;
    /// This node id
    uint8_t             _thisAddress;
    
    /// Whether the transport is in promiscuous mode
        bool           _promiscuous;
    /// Number of octets in the buffer
    uint8_t             _bufLen;
		/// The current transport operating mode
    volatile RHMode     _mode;
		
void nrf_init()
{
	/* Enable the GPIO Clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);
	 /* Configure the LEDs */
  GPIO_InitType GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = _chipEnablePin |  _txEnablePin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);
	/* Put the LEDs off */
  GPIO_WriteBit( _chipEnablePin, Bit_RESET);
	GPIO_WriteBit( _txEnablePin, Bit_RESET);
	
	
	SdkEvalSpiInit(100000);
	
	setModeIDLE();
		setRF(TransmitPowerm10dBm);
		setChannel(ch,FALSE);
	//spiWriteRegister(0x00,0x6C);
	spiWriteRegister(0x01,0x00);
	spiWriteRegister(0x02,0x44);spiWriteRegister(0x03,0x20);
	spiWriteRegister(0x04,0x20);spiWriteRegister(0x05,addr[0]);spiWriteRegister(0x06,addr[1]);spiWriteRegister(0x07,addr[2]);
	spiWriteRegister(0x08,addr[3]);spiWriteRegister(RH_NRF905_CONFIG_9, RH_NRF905_CONFIG_9_CRC_EN | RH_NRF905_CONFIG_9_CRC_MODE_16BIT | RH_NRF905_CONFIG_9_XOF_16MHZ);
	setPromiscuous(TRUE);
	//setModeIDLE();
	printf("NRF905 INITIALIZED\n");
}

void spiWriteRegister(uint8_t reg, uint8_t val)
{
   /* Set communication mode */
  SPI_SetMasterCommunicationMode(SPI_TRANSMIT_MODE);
	
	GPIO_ResetBits(SPI_CS_MS_DEMO_PIN);
	
	/* Write command to be send send to TX FIFO */
	while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE)){};
  SPI_SendData((reg & RH_NRF905_REG_MASK) | RH_NRF905_REG_W_CONFIG);
	while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE)){};
  SPI_SendData(val);
	
  /* Wait until data transfer is finished */
  while (SET == SPI_GetFlagStatus(SPI_FLAG_BSY));
	GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
}


uint8_t spiReadRegister(uint8_t reg)
{
    		/* Set communication mode */
		SPI_SetMasterCommunicationMode(SPI_FULL_DUPLEX_MODE);
	
    uint8_t val;
		GPIO_ResetBits(SPI_CS_MS_DEMO_PIN);
	
		/* Write data to send to TX FIFO */
		while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE));
		SPI_SendData((reg & RH_NRF905_REG_MASK) | RH_NRF905_REG_R_CONFIG);  
	
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

void validateRxBuf()
{
    // Check the length
    uint8_t len = _buf[4];
    if (len > RH_NRF905_MAX_MESSAGE_LEN)
			return; // Silly LEN header

    // Extract the 4 headers
    _rxHeaderTo    = _buf[0];
    _rxHeaderFrom  = _buf[1];
    _rxHeaderId    = _buf[2];
    _rxHeaderFlags = _buf[3];
    if (_promiscuous ||
	_rxHeaderTo == _thisAddress ||
	_rxHeaderTo == RH_BROADCAST_ADDRESS)
    {
	_rxGood++;
	_bufLen = len + RH_NRF905_HEADER_LEN; // _buf still includes the headers
	_rxBufValid = TRUE;
    }
}

BOOL NRF_data_available()
{
    if (!_rxBufValid)
    {
			if (_mode == RHModeTx)
					return FALSE;
			if (_mode != RHModeRx)
					{
						GPIO_WriteBit( _chipEnablePin |  _txEnablePin, Bit_SET);
						_mode = RHModeRx;
					}
					
// Get the message into the RX buffer, so we can inspect the headers
// we still dont know how long is the user message
			
			spiBurstRead(RH_NRF905_REG_R_RX_PAYLOAD, _buf, RH_NRF905_MAX_PAYLOAD_LEN);
					validateRxBuf();
			printf("%d\n",_rxBufValid); 
			if (_rxBufValid)
					setModeIDLE();
		}
    return _rxBufValid;
}

void tran_int(void)
{
		_mode = RHModeInitialising;
    _thisAddress = RH_BROADCAST_ADDRESS;
    _txHeaderTo = RH_BROADCAST_ADDRESS;
    _txHeaderFrom = RH_BROADCAST_ADDRESS;
		_txHeaderId = 0 ;
    _txHeaderFlags= 0 ;
    _rxBad = 0 ;
    _rxGood= 0 ;
    _txGood= 0 ;
	
}

void setPromiscuous(BOOL promiscuous)
{
    _promiscuous = promiscuous;
}

void setThisAddress(uint8_t address)
{
    _thisAddress = address;
}

void send_data(const uint8_t* data, uint8_t len)
{
	
	if (_mode != RHModeIdle)
    {
			GPIO_WriteBit( _chipEnablePin |  _txEnablePin, Bit_RESET);
			_mode = RHModeIdle;
    }	
	// Set up the headers
    _buf[0] = _txHeaderTo;
    _buf[1] = _txHeaderFrom;
    _buf[2] = _txHeaderId;
    _buf[3] = _txHeaderFlags;
    _buf[4] = len;
    memcpy(_buf+RH_NRF905_HEADER_LEN, data, len);
    spiBurstWrite(RH_NRF905_REG_W_TX_PAYLOAD, _buf, len + RH_NRF905_HEADER_LEN);
	
			GPIO_WriteBit( _txEnablePin, Bit_SET);
			GPIO_WriteBit( _chipEnablePin, Bit_SET);
			
			for(int i = 0; i < 1000; i++){;}
				
			GPIO_WriteBit( _txEnablePin, Bit_RESET);
			GPIO_WriteBit( _chipEnablePin, Bit_RESET);
		//spiBurstWrite(RH_NRF905_REG_W_TX_PAYLOAD, 0, 0);
				//_txGood++;	
}

BOOL data_recv(uint8_t* buf, uint8_t* len)
{
		if (!NRF_data_available())
	return FALSE;
    if (buf && len)
    {
	// Skip the 4 headers that are at the beginning of the rxBuf
	if (*len > _bufLen-RH_NRF905_HEADER_LEN)
	    *len = _bufLen-RH_NRF905_HEADER_LEN;
	memcpy(buf, _buf+RH_NRF905_HEADER_LEN, *len);
    }
    clearRxBuf(); // This message accepted and cleared
    return TRUE;
}

void clearRxBuf()
{
    _rxBufValid = FALSE;
    _bufLen = 0;
}

void setModeIDLE(void)
{
	 if (_mode != RHModeIdle)
    {
			GPIO_WriteBit( _txEnablePin, Bit_RESET);
			GPIO_WriteBit( _chipEnablePin, Bit_RESET);
			_mode = RHModeIdle;
    }	
}	

void setModeTX(void)
{
	if (_mode != RHModeTx)
    {
			GPIO_WriteBit( _txEnablePin, Bit_SET);
			GPIO_WriteBit( _chipEnablePin, Bit_SET);
			_mode = RHModeTx;
    }
}	

void setModeRX(void)
{
	if (_mode != RHModeRx)
    {
			GPIO_WriteBit( _txEnablePin, Bit_RESET);
			GPIO_WriteBit( _chipEnablePin, Bit_SET);
			_mode = RHModeRx;
    }	
}	

uint8_t ch_no(float frequency)
{
	uint8_t ch	= ((10 * frequency)/(1+0)) - (4224);
	return ch;
}

void setChannel(uint16_t channel, BOOL hiFrequency)
{
    spiWriteRegister(RH_NRF905_CONFIG_0, channel & RH_NRF905_CONFIG_0_CH_NO);
    // Set or clear the high bit of the channel
    uint8_t bit8 = (channel >> 8) & 0x01;
    uint8_t reg1 = spiReadRegister(RH_NRF905_CONFIG_1);
    reg1 = (reg1 & ~0x01) | bit8;
    // Set or clear the HFREQ_PLL bit
    reg1 &= ~RH_NRF905_CONFIG_1_HFREQ_PLL;
	printf("chanel config");
    if (hiFrequency)
		reg1 |= RH_NRF905_CONFIG_1_HFREQ_PLL;
    spiWriteRegister(RH_NRF905_CONFIG_1, reg1);
}

BOOL setRF(TransmitPower power)
{
    // Enum definitions of power are the same numerical values as the register
    uint8_t reg1 = spiReadRegister(RH_NRF905_CONFIG_1);
    reg1 &= ~RH_NRF905_CONFIG_1_PA_PWR;
    reg1 |= ((power & 0x3) << 2) & RH_NRF905_CONFIG_1_PA_PWR;
    spiWriteRegister(RH_NRF905_CONFIG_1, reg1);
    return TRUE;
}





void printRegisters()
{
    uint8_t registers[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};

    uint8_t i;
    for (i = 0; i < sizeof(registers); i++)
			printf("%d : %x\n", registers[i], spiReadRegister(registers[i]));
}

void setHeaderTo(uint8_t to)
{
    _txHeaderTo = to;
}

void setHeaderFrom(uint8_t from)
{
    _txHeaderFrom = from;
}

void setHeaderId(uint8_t id)
{
    _txHeaderId = id;
}

void setHeaderFlags(uint8_t set, uint8_t clear)
{
    _txHeaderFlags &= ~clear;
    _txHeaderFlags |= set;
}
