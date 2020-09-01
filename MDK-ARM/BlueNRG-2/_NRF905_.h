#ifndef ___NRF905__H__
#define ___NRF905__H__

#include "SPI_init.h"
#include "BlueNRG1_spi.h"
#include "stdio.h"
#include "BlueNRG1_gpio.h"
#include "stdlib.h"
#include "math.h"
#include "SDK_EVAL_SPI.h"
#include "stdbool.h"

#define RH_NRF905_MAX_PAYLOAD_LEN 32

// The length of the headers we add.
// The headers are inside the nRF905 payload
// As well as the usual TO, FROM, ID, FLAGS, we also need LEN, since
// nRF905 only has fixed width messages.
// REVISIT: could we have put the LEN into the FLAGS field?
#define RH_NRF905_HEADER_LEN 5

// This is the maximum RadioHead user message length that can be supported by this library. Limited by
// the supported message lengths in the nRF905
#define RH_NRF905_MAX_MESSAGE_LEN (RH_NRF905_MAX_PAYLOAD_LEN-RH_NRF905_HEADER_LEN)

// Register names
#define RH_NRF905_REG_MASK                   0x0f
#define RH_NRF905_REG_W_CONFIG               0x00
#define RH_NRF905_REG_R_CONFIG               0x10
#define RH_NRF905_REG_W_TX_PAYLOAD           0x20
#define RH_NRF905_REG_R_TX_PAYLOAD           0x21
#define RH_NRF905_REG_W_TX_ADDRESS           0x22
#define RH_NRF905_REG_R_TX_ADDRESS           0x23
#define RH_NRF905_REG_R_RX_PAYLOAD           0x24
#define RH_NRF905_REG_CHANNEL_CONFIG         0x80

// Configuration register
#define RH_NRF905_CONFIG_0                    0x00
#define RH_NRF905_CONFIG_0_CH_NO              0xff

#define RH_NRF905_CONFIG_1                    0x01
#define RH_NRF905_CONFIG_1_AUTO_RETRAN        0x20
#define RH_NRF905_CONFIG_1_RX_RED_PWR         0x10
#define RH_NRF905_CONFIG_1_PA_PWR             0x0c
#define RH_NRF905_CONFIG_1_PA_PWR_N10DBM      0x00
#define RH_NRF905_CONFIG_1_PA_PWR_N2DBM       0x04
#define RH_NRF905_CONFIG_1_PA_PWR_6DBM        0x08
#define RH_NRF905_CONFIG_1_PA_PWR_10DBM       0x0c
#define RH_NRF905_CONFIG_1_HFREQ_PLL          0x02
#define RH_NRF905_CONFIG_1_CH_NO              0x01

#define RH_NRF905_CONFIG_2                    0x02
#define RH_NRF905_CONFIG_2_TX_AFW             0x70
#define RH_NRF905_CONFIG_2_RX_AFW             0x07

#define RH_NRF905_CONFIG_3                    0x03
#define RH_NRF905_CONFIG_3_RX_PW              0x3f

#define RH_NRF905_CONFIG_4                    0x04
#define RH_NRF905_CONFIG_4_TX_PW              0x3f

#define RH_NRF905_CONFIG_5                    0x05
#define RH_NRF905_CONFIG_5_RX_ADDRESS         0xff

#define RH_NRF905_CONFIG_6                    0x06
#define RH_NRF905_CONFIG_6_RX_ADDRESS         0xff

#define RH_NRF905_CONFIG_7                    0x07
#define RH_NRF905_CONFIG_7_RX_ADDRESS         0xff

#define RH_NRF905_CONFIG_8                    0x08
#define RH_NRF905_CONFIG_8_RX_ADDRESS         0xff

#define RH_NRF905_CONFIG_9                    0x09
#define RH_NRF905_CONFIG_9_CRC_MODE_16BIT     0x80
#define RH_NRF905_CONFIG_9_CRC_EN             0x40
#define RH_NRF905_CONFIG_9_XOF                0x38
#define RH_NRF905_CONFIG_9_XOF_4MHZ           0x00
#define RH_NRF905_CONFIG_9_XOF_8MHZ           0x08
#define RH_NRF905_CONFIG_9_XOF_12MHZ          0x10
#define RH_NRF905_CONFIG_9_XOF_16MHZ          0x18
#define RH_NRF905_CONFIG_9_XOF_20MHZ          0x20
#define RH_NRF905_CONFIG_9_UP_CLK_EN          0x04
#define RH_NRF905_CONFIG_9_UP_CLK_FREQ        0x03
#define RH_NRF905_CONFIG_9_UP_CLK_FREQ_4MHZ   0x00
#define RH_NRF905_CONFIG_9_UP_CLK_FREQ_2MHZ   0x01
#define RH_NRF905_CONFIG_9_UP_CLK_FREQ_1MHZ   0x02
#define RH_NRF905_CONFIG_9_UP_CLK_FREQ_500KHZ 0x03

// This is the address that indicates a broadcast
#define RH_BROADCAST_ADDRESS 0xff

// Status register is always read as first byte
#define RH_NRF905_STATUS_AM                   0x80
#define RH_NRF905_STATUS_DR                   0x20

#define _chipEnablePin 	GPIO_Pin_13
#define _txEnablePin		GPIO_Pin_14
#define _DR		GPIO_Pin_10



typedef enum
    {
	TransmitPowerm10dBm = 0,  ///< -10 dBm
	TransmitPowerm2dBm,       ///< -2 dBm
	TransmitPower6dBm,        ///< 6 dBm
	TransmitPower10dBm        ///< 10 dBm
    } TransmitPower;
		
		
typedef enum
    {
	RHModeInitialising = 0, ///< Transport is initialising. Initial default value until init() is called..
	RHModeSleep,            ///< Transport hardware is in low power sleep mode (if supported)
	RHModeIdle,             ///< Transport is idle.
	RHModeTx,               ///< Transport is in the process of transmitting a message.
	RHModeRx,               ///< Transport is in the process of receiving a message.
	RHModeCad               ///< Transport is in the process of detecting channel activity (if supported)
    } RHMode;
		
void nrf_init(void);


		/// Reads a single register from the NRF905
    /// \param[in] reg Register number, one of NR905_REG_*
    /// \return The value of the register
uint8_t        spiReadRegister(uint8_t reg);

    /// Writes a single byte to the NRF905, and at the ame time reads the current STATUS register
    /// \param[in] reg Register number, one of NRF905_REG_*
    /// \param[in] val The value to write
    /// \return the current STATUS (read while the command is sent)
void        spiWriteRegister(uint8_t reg, uint8_t val);

void setChannel(uint16_t channel, BOOL hiFrequency);
BOOL setRF(TransmitPower power);
void printRegisters(void);
void send_data(const uint8_t* data, uint8_t len);

void tran_int(void);
void setHeaderTo(uint8_t to);
void setHeaderFrom(uint8_t from);
void setHeaderId(uint8_t id);
void setHeaderFlags(uint8_t set, uint8_t clear);

uint8_t status_read(void);

void setModeIDLE(void);
void setModeTx(void);
void setModeRx(void); 
void  RX_check();
void setPromiscuous(BOOL promiscuous);
void setThisAddress(uint8_t address);

bool NRF_data_available();
uint8_t ch_no(float frequency);
void clearRxBuf(void);
bool data_recv(uint8_t* buf, uint8_t* len);
unsigned char check_ready(void);

#endif
