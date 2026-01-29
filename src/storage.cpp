#include <SPI.h>
#include <SD.h>
#include <Arduino.h>

#include "storage.hpp"
#include "gps.hpp"
#include "sensors.hpp"
#include "display.hpp"


#define PIN_SD_SPI_CS    5

bool setupOK = false;

SPIClass *spiSD = NULL;

enum Display::StatusBarLevel status_sd = Display::STATUS_HIDE;

//helper Functions for FS access

#define FILE_DATA_PREFIX "/FLIGHT_DATA_"
#define FILE_LOG_PREFIX  "/LOG_"

String path_logFile = "---";
File file_log;
String path_dataFile = "---";
File file_data;

void sd_getValidPath(fs::SDFS &sd){
    int index = 0;
    while(true){
        String filename = FILE_DATA_PREFIX + (String)index + ".CSV";
        if(!sd.exists(filename)){
            path_dataFile = filename;
            path_logFile  = FILE_LOG_PREFIX + (String)index + ".CSV";
            return;
        }
        index ++;
        if(index > 9999){
            //no available index found (should never happen just in case something went wrong)
            Display::showMessage("SD ERROR: NO FILE INDEX", Display::MESSAGE_ERROR);
            while (1);
        }
    }
}

bool DisplayError_data = true;
void writeCSVLine(fs::SDFS &sd){
    String line;

    //timestamp
    line += (String) millis() + ",";
    //GPS
    line +=  String(GPS::GPS.location.lat(), 7) + ",";
    line +=  String(GPS::GPS.location.lng(), 7) + ",";
    line += (String) GPS::GPS.altitude.meters() + ",";
    line += (String) GPS::GPS.speed.kmph() + ",";
    line += (String) GPS::GPS.date.value() + ",";
    line += (String) GPS::GPS.time.value() + ",";
    line += (String) GPS::GPS.satellites.value() + ",";
    line += (String) GPS::GPS.hdop.hdop() + ",";
    //Sensors
    line += (String) Sensors::data.temp_ext_C + ",";
    line += (String) Sensors::data.temp_ext_C_2 + ",";
    line += (String) Sensors::data.pressure_ext + ",";
    line += (String) Sensors::data.humidity_ext + ",";
    line += (String) Sensors::data.uv_index;

    //write all data
    if(!SD.exists(path_dataFile)){
        if(DisplayError_data){
            Display::showMessage("SD ERROR: WRITE FAIL", Display::MESSAGE_ERROR);
        }
        DisplayError_data = false;
        Storage::init_storage(spiSD); //try to re-init SD
        return;
    }
    DisplayError_data = true;
    file_data.println(line);
    file_data.flush();
}
namespace Storage {

    bool DisplayError_log = true;
    void saveLogLine(String line){
        if(!setupOK) return;
        if(!SD.exists(path_logFile)){
            Display::showMessage("SD ERROR: WRITE FAIL", Display::MESSAGE_ERROR);
            Storage::init_storage(spiSD); //try to re-init SD
            DisplayError_log = false;
            return;
        }
        status_sd = Display::STATUS_OK;
        file_log.println(line);
        file_log.flush();
    }

    void getSpaceInfo(int *totalMB, int *usedMB){
        *totalMB = SD.cardSize() / (1024 * 1024);
        *usedMB = SD.usedBytes() / (1024 * 1024);
    }

    bool firstInit = true;
    void init_storage(SPIClass *spi){
        //store SPI instance for re-init if needed
        spiSD = spi;

        //same routine is used for re-init after failure
        if(firstInit){
            Display::addStatusBar(&status_sd, "SD");
        }

        if(!SD.begin(PIN_SD_SPI_CS, *spiSD)){
            status_sd = Display::STATUS_BAD;
            if(firstInit){
                Display::showMessage("SPI ERROR: SD CARD", Display::MESSAGE_ERROR);
                delay(1500);
            }
            return;
        }

        //autogen file paths
        if(firstInit){
            sd_getValidPath(SD);
            //create the file and check if that worked
            file_data = SD.open(path_dataFile, FILE_APPEND);
            file_log  = SD.open(path_logFile, FILE_APPEND);
            if(!file_data || !file_log){
                Display::showMessage("SD ERROR: CREATE FILE", Display::MESSAGE_ERROR);
                status_sd = Display::STATUS_BAD;
                delay(2000);
            }
            file_data.close();
            file_log.close();
        }

        
        //now open them for real
        file_data = SD.open(path_dataFile, FILE_APPEND);
        file_log  = SD.open(path_logFile, FILE_APPEND);
        if(!file_data || !file_log){
            status_sd = Display::STATUS_BAD;
            if(firstInit){
                Display::showMessage("SD ERROR: OPEN FILE", Display::MESSAGE_ERROR);
                delay(2000);
            }
            return;
        }

        //everything went well
        status_sd = Display::STATUS_OK;
        setupOK = true;
        if(firstInit){
            firstInit = false;
        }
    }




    void saveData(){
        if(!setupOK) return;  
        writeCSVLine(SD);
    }
}