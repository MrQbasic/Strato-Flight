#include "buzzer.hpp"
#include "Arduino.h"

TaskHandle_t buzzerTask = NULL;

void buzzer_init(){
    digitalWrite(BUZZER_PIN, 0);
    pinMode(BUZZER_PIN, OUTPUT);

    for(int i=0; i<3; i++){
        vTaskDelay(pdMS_TO_TICKS(100));
        digitalWrite(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        digitalWrite(BUZZER_PIN, 0);
    }

}

bool shouldRun = false;

void buzzer_task(void *paramters){
    while(shouldRun){
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(BUZZER_PIN, 0);
    }
    buzzerTask = NULL;
    vTaskDelete(NULL);
}

void buzzer_enable(){
    if(buzzerTask == NULL){
        shouldRun = true;
        xTaskCreatePinnedToCore(buzzer_task, "Buzzer", 1000, NULL, 1, &buzzerTask, 0);
    }   
}

void buzzer_disable(){
    shouldRun = false;
}