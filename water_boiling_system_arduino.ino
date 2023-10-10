#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "ansi.h"
#include "inttypes.h"

#define WATER_DRAIN_SPEED 30
#define WATER_BOILING_RATE 200
#define SENSOR_READING_TIMER 100
#define PRINTING_TIMER 160

ANSI ansi(&Serial);

//Tank 1 variables
uint8_t max_sensor_tank1 = 0, min_sensor_tank1 = 0;
uint8_t water_level_tank1 = 0;

//Tank 2 variables
uint8_t temp_water_tank2 = 27;
uint8_t max_sensor_tank2 = 0, min_sensor_tank2 = 0;
uint8_t water_level_tank2 = 0;

//Valve status:
uint8_t input_valve_status = 0, middle_valve_status = 0, output_valve_status = 0;

//Valves
SemaphoreHandle_t input_valve, middle_valve, output_valve, resistance;

void PrintSystemStatusTask() {
    for (;;) {
        ansi.foreground(ansi.white);
        ansi.gotoXY(0,0);
        ansi.print("TANK1: Water Level: ");
        ansi.print(water_level_tank1);
        ansi.print(" Max. Sensor: ");
        if(max_sensor_tank1){
            ansi.foreground(ansi.green);
        } else {
            ansi.foreground(ansi.red);
        }
        ansi.print(max_sensor_tank1);
        ansi.foreground(ansi.white);
        ansi.print(" Min. Sensor: ");
        if(min_sensor_tank1){
            ansi.foreground(ansi.green);
        } else {
            ansi.foreground(ansi.red);
        }
        ansi.print(min_sensor_tank1);
        ansi.foreground(ansi.white);
        ansi.println();
        ansi.print("TANK2: Water Level: ");
        ansi.print(water_level_tank2);
        ansi.print(" Max. Sensor: ");
        if(max_sensor_tank2){
            ansi.foreground(ansi.green);
        } else {
            ansi.foreground(ansi.red);
        }
        ansi.print(max_sensor_tank2);
        ansi.foreground(ansi.white);
        ansi.print(" Min. Sensor: ");
        if(min_sensor_tank2){
            ansi.foreground(ansi.green);
        } else {
            ansi.foreground(ansi.red);
        }
        ansi.print(min_sensor_tank2);
        ansi.foreground(ansi.white);
        ansi.print(" Temp: ");
        if(40 >= temp_water_tank2){
            ansi.foreground(ansi.blue);
        } else {
            if(40 < temp_water_tank2 && 65 >= temp_water_tank2){
                ansi.foreground(ansi.yellow);
            } else {
                if(65 < temp_water_tank2 && 70 >= temp_water_tank2){
                    ansi.foreground(ansi.red);
                }
            }
        }
        ansi.print(temp_water_tank2);
        ansi.println();
        if(input_valve_status){
            ansi.foreground(ansi.green);
            ansi.println("Input valve:ON ");
        } else {
            ansi.foreground(ansi.red);
            ansi.println("Input valve:OFF");
        }
        if(middle_valve_status){
            ansi.foreground(ansi.green);
            ansi.println("Middle valve:ON ");
        } else {
            ansi.foreground(ansi.red);
            ansi.println("Middle valve:OFF");
        }
        vTaskDelay(PRINTING_TIMER / portTICK_PERIOD_MS);
    }
}

void ReadDataFromSensors(){
    //Water Tank 01 readings
    if (100 <= water_level_tank1) {
        max_sensor_tank1 = 1;
    } else {
        max_sensor_tank1 = 0;
        if (20 <= water_level_tank1) {
            min_sensor_tank1 = 1;
        } else {
            min_sensor_tank1 = 0;
        }
    }
    //Water tank 02 readings
    if (100 <= water_level_tank2) {
        max_sensor_tank2 = 1;
    } else {
        max_sensor_tank2 = 0;
        if(20 <= water_level_tank2){
            min_sensor_tank2 = 1;
        } else {
            min_sensor_tank2 = 0;
        }
    }
}

void InputValveControlTask(){
    for(;;){
        xSemaphoreTake(input_valve, portMAX_DELAY);
        water_level_tank1 += 2;
        vTaskDelay(WATER_DRAIN_SPEED / portTICK_PERIOD_MS);
    }
}

void MiddleValveControlTask(){
    for(;;){
        xSemaphoreTake(middle_valve, portMAX_DELAY);
        water_level_tank2++;
        water_level_tank1--;
        if(27 < temp_water_tank2){
            temp_water_tank2--;
        }
        vTaskDelay(WATER_DRAIN_SPEED / portTICK_PERIOD_MS);
    }
}

void SystemControlTask(){
    for(;;){
        input_valve_status = 0;
        middle_valve_status = 0;
        ReadDataFromSensors();
        //Control Logic
        if(1 != max_sensor_tank1){
            input_valve_status = 1;
            xSemaphoreGive(input_valve);
        }
        if(1 != max_sensor_tank2){
            middle_valve_status = 1;
            xSemaphoreGive(middle_valve);
        }
        if(1 == max_sensor_tank2 && 70 > temp_water_tank2){
            xSemaphoreGive(resistance);
        }
        vTaskDelay(SENSOR_READING_TIMER / portTICK_PERIOD_MS);
    }
}

void ResistanceControlTask(){
    for(;;) {
        xSemaphoreTake(resistance, portMAX_DELAY);
        temp_water_tank2++;
        vTaskDelay(WATER_BOILING_RATE / portTICK_PERIOD_MS);
    }
}

void OutputValveControlTask(){
    for(;;) {
        water_level_tank2--;
        vTaskDelay(WATER_DRAIN_SPEED / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(9600);
    input_valve = xSemaphoreCreateMutex();
    if (input_valve != NULL) {
        ansi.println("Input valve mutex created");
    }
    middle_valve = xSemaphoreCreateMutex();
    if (middle_valve != NULL) {
        ansi.println("Middle valve mutex created");
    }

    // output_valve = xSemaphoreCreateMutex();
    // if (output_valve != NULL) {
    //     ansi.println("Output valve mutex created");
    // }

    resistance = xSemaphoreCreateMutex();
    if (resistance != NULL) {
        ansi.println("Resistance mutex created");
    }

    ansi.foreground(ansi.white);
    ansi.clearScreen();
    xTaskCreate(SystemControlTask,"SystemControlTask",80,NULL,1,NULL);
    xTaskCreate(InputValveControlTask,"InputValveControlTask",80,NULL,3,NULL);
    xTaskCreate(MiddleValveControlTask,"MiddleValveControlTask",80,NULL,3,NULL);
    xTaskCreate(ResistanceControlTask,"ResistanceControlTask",80,NULL,3,NULL);
    // xTaskCreate(OutputValveControlTask,"OutputValveControlTask",80,NULL,4,NULL);
    xTaskCreate(PrintSystemStatusTask,"PrintSystemStatus",100,NULL,5,NULL);
}

void loop() {}