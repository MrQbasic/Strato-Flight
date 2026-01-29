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
SPISettings lora_spi_Settings(8000000, MSBFIRST, SPI_MODE0);
SPIClass    *loraSPI = NULL;


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


void wireless_lora_wait(){
    while(digitalRead(LORA_SPI_BUSY_PIN) == HIGH){
        yield();
    }
}

void wireless_lora_reset(){
    //trigger reset
    digitalWrite(LORA_SPI_RESET_PIN, LOW);
    delay(100);
    digitalWrite(LORA_SPI_RESET_PIN, HIGH);
    delay(100);
    //wait for ok
    wireless_lora_wait();
}

void wireless_lora_sendCommand(uint8_t cmd, uint8_t* data, uint8_t length){
    wireless_lora_wait();
    loraSPI->beginTransaction(lora_spi_Settings);
    digitalWrite(LORA_SPI_NSS_PIN, LOW);
    loraSPI->transfer(cmd);
    for(int i=0; i<length; i++){
        loraSPI->transfer(data[i]);
    }
    digitalWrite(LORA_SPI_NSS_PIN, HIGH);
    loraSPI->endTransaction();
}

void wireless_lora_readCommand(uint8_t cmd, uint8_t* data, uint8_t length){
    wireless_lora_wait();
    loraSPI->beginTransaction(lora_spi_Settings);
    digitalWrite(LORA_SPI_NSS_PIN, LOW);
    loraSPI->transfer(cmd);
    for(int i=0; i<length; i++){
        data[i] = loraSPI->transfer(0x00);
    }
    digitalWrite(LORA_SPI_NSS_PIN, HIGH);
    loraSPI->endTransaction();
}

void wireless_lora_writeBuffer(uint8_t offset, uint8_t* data, uint8_t length){
    wireless_lora_wait();
    loraSPI->beginTransaction(lora_spi_Settings);
    digitalWrite(LORA_SPI_NSS_PIN, LOW);
    loraSPI->transfer(LORA_CMD_WRITE_BUFFER);
    loraSPI->transfer(offset);
    for(int i=0; i<length; i++){
        loraSPI->transfer(data[i]);
    }
    digitalWrite(LORA_SPI_NSS_PIN, HIGH);
    loraSPI->endTransaction();
}

void wireless_lora_readBuffer(uint8_t offset, uint8_t* data, uint8_t length){
    wireless_lora_wait();
    loraSPI->beginTransaction(lora_spi_Settings);
    digitalWrite(LORA_SPI_NSS_PIN, LOW);
    loraSPI->transfer(LORA_CMD_READ_BUFFER);
    loraSPI->transfer(offset);
    for(int i=0; i<length; i++){
        data[i] = loraSPI->transfer(0x00);
    }
    digitalWrite(LORA_SPI_NSS_PIN, HIGH);
    loraSPI->endTransaction();
}

void wireless_lora_clearErrors(){
    uint8_t buf[2];
    buf[0] = 0x00;
    buf[1] = 0x00;
    wireless_lora_sendCommand(0x07, buf, 2);
}

void wireless_lora_write_Register(uint16_t address, uint8_t* data, uint8_t length){
    uint8_t buf[3 + length];
    buf[0] = (address >> 8) & 0xFF;
    buf[1] = address & 0xFF;
    for(int i=0; i<length; i++){
        buf[2 + i] = data[i];
    }
    wireless_lora_sendCommand(0x0D, buf, 3 + length);
}

bool wireless_lora_init(SPIClass *spi){
    // Initialize Pins
    pinMode(LORA_SPI_NSS_PIN, OUTPUT);
    digitalWrite(LORA_SPI_NSS_PIN, HIGH);
    pinMode(LORA_SPI_RESET_PIN, OUTPUT);
    wireless_lora_reset();
    pinMode(LORA_SPI_BUSY_PIN, INPUT);
    pinMode(LORA_SPI_DIO1_PIN, INPUT);
    // Initialize SPI
    loraSPI = spi;

    uint8_t buf[32];
    //set standbyMode
    buf[0] = 0x00; //Standby RC
    wireless_lora_sendCommand(LORA_CMD_SET_STANDBY, buf, 1);
    //setup TCX0
    buf[0] = 0x02; //TCXO voltage to 3.0V
    buf[1] = 0x00; 
    buf[2] = 0x01; 
    buf[3] = 0x40;
    wireless_lora_sendCommand(LORA_CMD_SET_TCXO_CONTROL, buf, 4);
    //Calibrate 
    buf[0] = 0x7F; //Calibrate all
    wireless_lora_sendCommand(LORA_CMD_CALIBRATE, buf, 1);
    //set dio2 as rf switch
    buf[0] = 0x01;
    wireless_lora_sendCommand(LORA_CMD_SET_DIO2_AS_RF_SWITCH, buf, 1);
    //set regulator mode
    buf[0] = 0x01; //LDO
    wireless_lora_sendCommand(LORA_CMD_SET_REGULATOR_MODE, buf, 1);
    //set buffer base address
    buf[0] = 0x00; //Tx base addr
    buf[1] = 0x00; //Rx base addr
    wireless_lora_sendCommand(LORA_CMD_SET_BUFFER_BASE_ADDRESS, buf, 2);
    //set packet type
    buf[0] = 0x01; //Packet type LoRa
    wireless_lora_sendCommand(LORA_CMD_SET_PACKETTYPE, buf, 1);
    //set PA config
    buf[0] = 0x04; //PA duty cycle
    buf[1] = 0x07; //HP max
    buf[2] = 0x00; //devive select: SX1262
    buf[3] = 0x01; //PALUT
    wireless_lora_sendCommand(LORA_CMD_SET_PA_CONFIG, buf, 4);
    //set OCP
    buf[0] = 0x38;
    wireless_lora_write_Register(LORA_REG_OCP, &buf[0], 1);
    //set tx params
    buf[0] = LORA_TX_OUTPUT_POWER; //Output power
    buf[1] = 0x07;                 //Ramp time 3400us
    wireless_lora_sendCommand(LORA_CMD_SET_TX_PARAMS, buf, 2);
    //calibrate image
    buf[0] = 0xE1;
    buf[1] = 0xE9;
    wireless_lora_sendCommand(LORA_CMD_CALIBRATE_IMAGE, buf, 2);
    //set frequency
    uint64_t rfFreq = ((uint64_t)LORA_RF_FREQUENCY << 25) / LORA_RF_XTAL;
    buf[0] = (uint8_t)((rfFreq >> 24) & 0xFF);
    buf[1] = (uint8_t)((rfFreq >> 16) & 0xFF);
    buf[2] = (uint8_t)((rfFreq >> 8)  & 0xFF);
    buf[3] = (uint8_t)((rfFreq >> 0)  & 0xFF);
    wireless_lora_sendCommand(LORA_CMD_SET_RF_FREQUENCY, buf, 4);
    //set packet type
    buf[0] = 0x01; //LORA
    wireless_lora_sendCommand(LORA_CMD_SET_PACKETTYPE, buf, 1);
    //set modulation params
    buf[0] = LORA_SPREADING_FACTOR; //Spreading Factor
    buf[1] = LORA_BANDWIDTH;
    buf[2] = LORA_CODINGRATE;       //Coding Rate
    buf[3] = 0x00;                  //Low Data Rate Optimize OFF LDRO
    wireless_lora_sendCommand(LORA_CMD_SET_MODULATION_PARAM, buf, 4);

    return false;
}

void wireless_lora_printErrors(){
    uint8_t buf[3];
    wireless_lora_readCommand(LORA_CMD_GET_ERRORS, buf, 3);
    Serial.print("LORA ERRORS: ");
    Serial.print(buf[0], HEX);
    Serial.print(" ");
    Serial.print(buf[1], HEX);
    Serial.print(" ");
    Serial.println(buf[2], HEX);
}

void wireless_lora_printStatus(){
    uint8_t buf[1];
    wireless_lora_readCommand(LORA_CMD_GET_STATUS, buf, 1);
    Serial.print("LORA STATUS: ");
    Serial.println(buf[0], HEX);
}

bool wireless_lora_sendData(char* data, int length){
    uint8_t buf[6];
    //set packet params
    buf[0] = LORA_PREAMBLE_LENGTH >> 8;
    buf[1] = LORA_PREAMBLE_LENGTH & 0xFF;
    buf[2] = 0x00;      //No implicit header
    buf[3] = length;    //Payload Length
    buf[4] = 0x01;      //Enable CRC
    buf[5] = 0x00;      //IQ standard
    wireless_lora_sendCommand(LORA_CMD_SET_PACKET_PARMS, buf, 6);
    //write data to buffer
    wireless_lora_writeBuffer(0x00, (uint8_t*)data, length);
    //send the packet
    buf[0] = 0x00; 
    buf[1] = 0x00;
    buf[2] = 0x00;
    wireless_lora_sendCommand(LORA_CMD_SET_TX, buf, 3);
    return false;
}