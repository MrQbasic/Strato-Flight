#include <SPI.h>
#include <Arduino.h>
#include "display.hpp"

#define LORA_RF_XTAL                                32000000
#define LORA_RF_FREQUENCY                           868125000 // Hz
#define LORA_TX_OUTPUT_POWER                        5        // dBm
#define LORA_BANDWIDTH                              4        // [4: 125 kHz,
                                                              //  5: 250 kHz,
                                                              //  6: 500 kHz,
#define LORA_SPREADING_FACTOR                       7         // [SF5..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define LORA_RX_TIMEOUT_VALUE                       1000


#define LORA_SPI_NSS_PIN         8
#define LORA_SPI_BUSY_PIN       13
#define LORA_SPI_RESET_PIN      12
#define LORA_SPI_DIO1_PIN       14


#define LORA_CMD_SET_STANDBY                0x80
#define LORA_CMD_SET_PACKETTYPE             0x8A
#define LORA_CMD_SET_RF_FREQUENCY           0x86
#define LORA_CMD_SET_PA_CONFIG              0x95
#define LORA_CMD_SET_TX_PARAMS              0x8E
#define LORA_CMD_SET_MODULATION_PARAM       0x8B
#define LORA_CMD_SET_PACKET_PARMS           0x8C
#define LORA_CMD_SET_DIO_IRQ_PARAMS         0x08
#define LORA_CMD_SET_TCXO_CONTROL           0x97
#define LORA_CMD_SET_TX                     0x83
#define LORA_CMD_CALIBRATE                  0x89
#define LORA_CMD_CALIBRATE_IMAGE            0x98
#define LORA_CMD_SET_DIO2_AS_RF_SWITCH      0x9D
#define LORA_CMD_SET_REGULATOR_MODE         0x96
#define LORA_CMD_SET_BUFFER_BASE_ADDRESS    0x8F
#define LORA_CMD_GET_ERRORS                 0x17
#define LORA_CMD_GET_STATUS                 0xC0

#define LORA_REG_OCP                        0x08E7

#define LORA_CMD_WRITE_BUFFER           0x0E
#define LORA_CMD_READ_BUFFER            0x1E

bool wireless_lora_init(SPIClass *spi);

bool wireless_lora_sendData(char* data, int length);