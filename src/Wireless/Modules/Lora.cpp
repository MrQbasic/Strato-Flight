#include "Lora.hpp"

SPISettings lora_spi_Settings(8000000, MSBFIRST, SPI_MODE0);
SPIClass    *loraSPI = NULL;

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