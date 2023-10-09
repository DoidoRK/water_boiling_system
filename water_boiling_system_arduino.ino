#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "ansi.h"
#include "inttypes.h"

#define WATER_DRAIN_SPEED 30
#define WATER_BOILING_RATE 200
#define SENSOR_READING_TIMER 100

ANSI ansi(&Serial);

//Tank 1 variables
uint8_t max_sensor_tank1 = 0, min_sensor_tank1 = 0;
uint8_t water_level_tank1 = 0;

//Tank 2 variables
uint8_t temp_water_tank2 = 20;
uint8_t max_sensor_tank2 = 0, min_sensor_tank2 = 0;
uint8_t water_level_tank2 = 0;

//Valves
SemaphoreHandle_t input_valve, middle_valve, output_valve, resistance;

void PrintSystemStatusTask() {
    while (1) {
        Ansi.clearScreen();
        Ansi.print("TANK1: Water Level: ");
        Ansi.print(water_level_tank1);
        Ansi.print(" Max. Sensor: ");
        Ansi.print(max_sensor_tank1);
        Ansi.print(" Min. Sensor: ");
        Ansi.print(min_sensor_tank1);
        Ansi.println();
        Ansi.print("TANK2: Water Level: ");
        Ansi.print(water_level_tank2);
        Ansi.print(" Max. Sensor: ");
        Ansi.print(max_sensor_tank2);
        Ansi.print(" Min. Sensor: ");
        Ansi.print(min_sensor_tank2);
        Ansi.print(" Temp: ");
        Ansi.print(temp_water_tank2);
        vTaskDelay(SENSOR_READING_TIMER / portTICK_PERIOD_MS);
    }
}


void InputValveControlTask(){
    while (1) {
        xSemaphoreTake(input_valve, portMAX_DELAY);
        water_level_tank1++;
        vTaskDelay(WATER_DRAIN_SPEED / portTICK_PERIOD_MS);
    }
}

void Tank1SensorsReadingsTask() {
    xSemaphoreGive(input_valve);
    while (1) {
        if (100 <= water_level_tank1) {
            max_sensor_tank1 = 1;
            xSemaphoreTake(input_valve, portMAX_DELAY);
        } else {
            max_sensor_tank1 = 0;
            if (20 <= water_level_tank1) {
                min_sensor_tank1 = 1;
            } else {
                min_sensor_tank1 = 0;
            }
            xSemaphoreGive(input_valve);
        }
    }
}

void MiddleValveControlTask(){
    while (1) {
        xSemaphoreTake(middle_valve, portMAX_DELAY);
        if(water_level_tank1 > 0){
            water_level_tank1--;
            water_level_tank2++;
        }
        // if(temp_water_tank2 > 20){
        //     temp_water_tank2--;
        // }
        vTaskDelay(WATER_DRAIN_SPEED / portTICK_PERIOD_MS);
    }
}

void Tank2SensorsReadingsTask() {
    while (1) {
        //Water level sensors
        if (100 <= water_level_tank2) {
            max_sensor_tank2 = 1;
            xSemaphoreTake(middle_valve, portMAX_DELAY);
        } else {
            max_sensor_tank2 = 0;
            if( 20 <= water_level_tank2){
                min_sensor_tank2 = 1;
            } else {
                min_sensor_tank2 = 0;
            }
            xSemaphoreGive(middle_valve);
        }
        //Temperature Sensor
        // if(70 >= temp_water_tank2 && max_sensor_tank2 != 1){
        //     xSemaphoreTake(resistance, portMAX_DELAY);
        // } else {
        //     xSemaphoreGive(resistance);
        // }
    }
}

void ResistanceControlTask(){
    while (1)
    {
        xSemaphoreTake(resistance, portMAX_DELAY);
        temp_water_tank2++;
        vTaskDelay(WATER_BOILING_RATE / portTICK_PERIOD_MS);
    }
}

void OutputValveControlTask(){
    while (1) {
        xSemaphoreTake(output_valve, portMAX_DELAY);
        water_level_tank2--;
        vTaskDelay(WATER_DRAIN_SPEED / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    Ansi.normal();
    Ansi.clearScreen();
    input_valve = xSemaphoreCreateMutex();
    if (input_valve != NULL) {
        ansi.print("Input valve mutex created");
    }
    middle_valve = xSemaphoreCreateMutex();
    if (middle_valve != NULL) {
        ansi.print("Middle valve mutex created");
    }
    output_valve = xSemaphoreCreateMutex();
    if (output_valve != NULL) {
        ansi.print("Output valve mutex created");
    }
    resistance = xSemaphoreCreateMutex();
    if (resistance != NULL) {
        ansi.print("Resistance mutex created");
    }
    xTaskCreate(Tank1SensorsReadingsTask,"Tank1SensorReadings",80,NULL,1,NULL);
    xTaskCreate(Tank2SensorsReadingsTask,"Tank2SensorReadings",80,NULL,1,NULL);
    xTaskCreate(InputValveControlTask,"InputValveControlTask",80,NULL,3,NULL);
    xTaskCreate(MiddleValveControlTask,"MiddleValveControlTask",80,NULL,3,NULL);
    xTaskCreate(OutputValveControlTask,"OutputValveControlTask",80,NULL,3,NULL);
    xTaskCreate(PrintSystemStatusTask,"PrintSystemStatus",80,NULL,5,NULL);
}

void loop() {}