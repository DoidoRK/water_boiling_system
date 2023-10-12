#include <stdio.h>
#include "conio_linux.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "system_print.h"

#define WATER_DRAIN_SPEED 150
#define WATER_BOILING_RATE 150
#define SENSOR_READING_TIMER 100
#define PRINTING_TIMER 160
#define WATER_MAX_LEVEL 95
#define WATER_MIN_LEVEL 20

//Tank 1 variables
uint8_t max_sensor_tank1 = 0, min_sensor_tank1 = 0;
uint8_t water_level_tank1 = 0;

//Tank 2 variables
uint8_t temp_water_tank2 = 27;
uint8_t max_sensor_tank2 = 0, min_sensor_tank2 = 0;
uint8_t water_level_tank2 = 0;

//System actuators status:
uint8_t input_valve_status = 0, middle_valve_status = 0, output_valve_status = 0, resistance_status = 0;

//Water state status:
uint8_t water_is_boiled = 0;

//Valves
SemaphoreHandle_t water_tank1_mutex, water_tank2_mutex, temp_water2_mutex;

void print_boolean_sensor_data(uint8_t boolean_data){
    if(boolean_data){
            setfontcolor(GREEN);
        } else {
            setfontcolor(RED);
        }
    printf("%s", boolean_data? "ON " : "OFF");
    setfontcolor(WHITE);
}

void print_number_sensor_data(uint8_t sensor_data){
    if(sensor_data){
        setfontcolor(GREEN);
    } else {
        setfontcolor(RED);
    }
    printf("%d ", sensor_data);
    setfontcolor(WHITE);
}

void PrintSystemStatusTask() {
    for (;;) {
        setfontcolor(WHITE);
        gotoxy(0,0);
        printf("--------Water Tank 1--------\n");
        printf("Water Level: %d ", water_level_tank1);
        printf("\n");
        printf("Max. Level Sensor: ");
        print_number_sensor_data(max_sensor_tank1);
        printf("\n");
        printf("Min. Level Sensor: ");
        print_number_sensor_data(min_sensor_tank1);
        printf("\n");
        printf("----------------------------\n");

        printf("--------Water Tank 2--------\n");
        printf("Water Level: %d ", water_level_tank2);
        printf("\n");
        printf("Max. Level Sensor: ");
        print_number_sensor_data(max_sensor_tank2);
        printf("\n");
        printf("Min. Level Sensor: ");
        print_number_sensor_data(min_sensor_tank2);
        printf("\n");
        printf("Water Temperature: ");
        if(40 >= temp_water_tank2){
            setfontcolor(BLUE);
        } else {
            if(40 < temp_water_tank2 && 65 >= temp_water_tank2){
                setfontcolor(YELLOW);
            } else {
                if(65 < temp_water_tank2){
                    setfontcolor(RED);
                }
            }
        }
        printf("%d°C\n", temp_water_tank2);
        setfontcolor(WHITE);
        printf("----------------------------\n");

        printf("--------System Valves-------\n");
        printf("Input Valve: ");
        print_boolean_sensor_data(input_valve_status);
        printf("\n");
        printf("Middle Valve: ");
        print_boolean_sensor_data(middle_valve_status);
        printf("\n");
        printf("Resistance: ");
        print_boolean_sensor_data(resistance_status);
        printf("\n");
        printf("Output Valve: ");
        print_boolean_sensor_data(output_valve_status);
        printf("\n");
        printf("----------------------------\n");
        valve_print(40,0,input_valve_status,27);
        tank_print(40,0,water_level_tank1,27,0,0,max_sensor_tank1,min_sensor_tank1);
        valve_print(61,0,middle_valve_status,27);
        tank_print(61,0,water_level_tank2,temp_water_tank2,resistance_status,2,max_sensor_tank2,min_sensor_tank2);
        valve_print(82,0,output_valve_status,temp_water_tank2);
        gotoxy(0,18);
        vTaskDelay(pdMS_TO_TICKS(PRINTING_TIMER));
    }
}

void ReadDataFromSensors(){
    //Water Tank 01 readings
    if (WATER_MAX_LEVEL <= water_level_tank1) {
        max_sensor_tank1 = 1;
    } else {
        max_sensor_tank1 = 0;
        if (WATER_MIN_LEVEL <= water_level_tank1) {
            min_sensor_tank1 = 1;
        } else {
            min_sensor_tank1 = 0;
        }
    }
    // Water tank 02 readings
    if (WATER_MAX_LEVEL <= water_level_tank2) {
        max_sensor_tank2 = 1;
    } else {
        max_sensor_tank2 = 0;
        if(WATER_MIN_LEVEL <= water_level_tank2){
            min_sensor_tank2 = 1;
        } else {
            min_sensor_tank2 = 0;
        }
    }
}

void InputValveControlTask(){
    for(;;){
        if(input_valve_status){
            xSemaphoreTake(water_tank1_mutex, portMAX_DELAY);
                water_level_tank1 += 2;
            xSemaphoreGive(water_tank1_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(WATER_DRAIN_SPEED));
    }
}

void MiddleValveControlTask(){
    for(;;){
        if(middle_valve_status){

            xSemaphoreTake(water_tank1_mutex, portMAX_DELAY);
                water_level_tank1--;
            xSemaphoreGive(water_tank1_mutex);

            xSemaphoreTake(water_tank2_mutex, portMAX_DELAY);
                water_level_tank2++;
            xSemaphoreGive(water_tank2_mutex);

            if(27 < temp_water_tank2){
                xSemaphoreTake(temp_water2_mutex, portMAX_DELAY);
                    temp_water_tank2--;
                xSemaphoreGive(temp_water2_mutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(WATER_DRAIN_SPEED));
    }
}

void ResistanceControlTask(){
    for(;;) {
        if(resistance_status){
            xSemaphoreTake(temp_water2_mutex, portMAX_DELAY);
                temp_water_tank2++;
            xSemaphoreGive(temp_water2_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(WATER_BOILING_RATE));
    }
}

void OutputValveControlTask(){
    for(;;) {
        if(output_valve_status){
            xSemaphoreTake(water_tank2_mutex, portMAX_DELAY);
                water_level_tank2--;
            xSemaphoreGive(water_tank2_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(WATER_DRAIN_SPEED));
    }
}

void SystemControlTask(){
    for(;;){
        input_valve_status = 0;
        middle_valve_status = 0;
        output_valve_status = 0;
        water_is_boiled = 0;
        
        ReadDataFromSensors();

        if(1 != max_sensor_tank1){
            input_valve_status = 1;
        }

        if(70 <= temp_water_tank2){
            resistance_status = 0;
            water_is_boiled = 1;
        }

        if(1 == max_sensor_tank2 && 70 > temp_water_tank2){
            resistance_status = 1;
        }

        if(!min_sensor_tank2){
            water_is_boiled = 0;
        }

        if(1 != max_sensor_tank2 && water_is_boiled != 1){
            middle_valve_status = 1;
        }

        if(70 <= temp_water_tank2 && min_sensor_tank2 != 0){
            output_valve_status = 1;
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_READING_TIMER));
    }
}

void app_main() {
    water_tank1_mutex = xSemaphoreCreateMutex();
    if (water_tank1_mutex != NULL) {
        printf("Input valve mutex created\n");
    }
    xSemaphoreGive(water_tank1_mutex);

    water_tank2_mutex = xSemaphoreCreateMutex();
    if (water_tank2_mutex != NULL) {
        printf("Middle valve mutex created\n");
    }
    xSemaphoreGive(water_tank2_mutex);

    temp_water2_mutex = xSemaphoreCreateMutex();
    if (temp_water2_mutex != NULL) {
        printf("Resistance mutex created\n");
    }
    xSemaphoreGive(temp_water2_mutex);

    setfontcolor(WHITE);
    clrscr();
    printf("Limpou a tela\n");
    xTaskCreate(SystemControlTask,"SystemControlTask", 2048, NULL, 1, NULL);
    xTaskCreate(InputValveControlTask,"InputValveControlTask",2048,NULL,3,NULL);
    xTaskCreate(MiddleValveControlTask,"MiddleValveControlTask",2048,NULL,3,NULL);
    xTaskCreate(ResistanceControlTask,"ResistanceControlTask",2048,NULL,3,NULL);
    xTaskCreate(OutputValveControlTask,"OutputValveControlTask",2048,NULL,4,NULL);
    xTaskCreate(PrintSystemStatusTask,"PrintSystemStatus",2048,NULL,5,NULL);
}