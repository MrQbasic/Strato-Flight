#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

#include "sensors.h"
#include "display.h"

void app_main(void){
    vTaskDelay(pdMS_TO_TICKS(5000)); //wait for sensor to power up
    printf("Starting main task\n");

    //sensors_init();
    //TODO: error Handling
    //sensors_start();    

    display_init();

    char stats_buffer[1024];
    while(1) {
        vTaskGetRunTimeStats(stats_buffer);
        //printf("%s\n", stats_buffer);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}