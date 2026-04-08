#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

#include "sensors.h"

void app_main(void){

    sensors_init();

    char stats_buffer[1024];
    while(1) {
        vTaskGetRunTimeStats(stats_buffer);
        printf("%s\n", stats_buffer);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}