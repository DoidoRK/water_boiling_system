#include "types.h"
#include <stdio.h>
#include "conio_linux.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

int input_valve_flow_speed = 150;
int middle_valve_flow_speed = 150;
int output_valve_flow_speed = 150;
int water_boiling_rate = 150;
int sensor_reading_timer = 100;
int water_tank_water_max_level = 95;
int water_tank_water_min_level = 20;
int boiling_tank_water_max_level = 95;
int boiling_tank_water_min_level = 20;

//Tank 1 variables
int max_sensor_tank1 = 0, min_sensor_tank1 = 0;
int water_level_tank1 = 0;

//Tank 2 variables
int temp_water_tank2 = 27;
int max_sensor_tank2 = 0, min_sensor_tank2 = 0;
int water_level_tank2 = 0;

//System actuators status:
int input_valve_status = 0, middle_valve_status = 0, output_valve_status = 0, resistance_status = 0;

//Water state status:
int water_is_boiled = 0;

//Valves
SemaphoreHandle_t water_tank1_mutex, water_tank2_mutex, temp_water2_mutex;

void ReadDataFromSensors(){
    //Water Tank 01 readings
    if (water_tank_water_max_level <= water_level_tank1) {
        max_sensor_tank1 = 1;
    } else {
        max_sensor_tank1 = 0;
        if (water_tank_water_min_level <= water_level_tank1) {
            min_sensor_tank1 = 1;
        } else {
            min_sensor_tank1 = 0;
        }
    }
    // Water tank 02 readings
    if (boiling_tank_water_max_level <= water_level_tank2) {
        max_sensor_tank2 = 1;
    } else {
        max_sensor_tank2 = 0;
        if(boiling_tank_water_min_level <= water_level_tank2){
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
        vTaskDelay(pdMS_TO_TICKS(input_valve_flow_speed));
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
        vTaskDelay(pdMS_TO_TICKS(middle_valve_flow_speed));
    }
}

void ResistanceControlTask(){
    for(;;) {
        if(resistance_status){
            xSemaphoreTake(temp_water2_mutex, portMAX_DELAY);
                temp_water_tank2++;
            xSemaphoreGive(temp_water2_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(water_boiling_rate));
    }
}

void OutputValveControlTask(){
    for(;;) {
        if(output_valve_status){
            xSemaphoreTake(water_tank2_mutex, portMAX_DELAY);
                water_level_tank2--;
            xSemaphoreGive(water_tank2_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(output_valve_flow_speed));
    }
}

void SystemControlTask(){
    for(;;){
        sensor_readings_t sensor_readings;
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

        sensor_readings.max_sensor_tank1 = max_sensor_tank1;
        sensor_readings.min_sensor_tank1 = min_sensor_tank1;
        sensor_readings.water_level_tank1 = water_level_tank1;
        sensor_readings.temp_water_tank2 = temp_water_tank2;
        sensor_readings.max_sensor_tank2 = max_sensor_tank2;
        sensor_readings.min_sensor_tank2 = min_sensor_tank2;
        sensor_readings.water_level_tank2 = water_level_tank2;
        sensor_readings.input_valve_status = input_valve_status;
        sensor_readings.middle_valve_status = middle_valve_status;
        sensor_readings.output_valve_status = output_valve_status;
        sensor_readings.resistance_status = resistance_status;
        sensor_readings.water_is_boiled = water_is_boiled;

        xQueueSend(sensor_readings_queue, &sensor_readings, pdMS_TO_TICKS(sensor_reading_timer));

        vTaskDelay(pdMS_TO_TICKS(sensor_reading_timer));
    }
}

void startup_system(){
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
    xTaskCreate(SystemControlTask,"SystemControlTask", 2048, NULL, 1, NULL);
    xTaskCreate(InputValveControlTask,"InputValveControlTask",2048,NULL,3,NULL);
    xTaskCreate(MiddleValveControlTask,"MiddleValveControlTask",2048,NULL,3,NULL);
    xTaskCreate(ResistanceControlTask,"ResistanceControlTask",2048,NULL,3,NULL);
    xTaskCreate(OutputValveControlTask,"OutputValveControlTask",2048,NULL,4,NULL);
}

void set_system_parameters(system_params_t system_settings) {
    int input_valve_flow_speed = system_settings.input_valve_flow_speed;
    int middle_valve_flow_speed = system_settings.middle_valve_flow_speed;
    int output_valve_flow_speed = system_settings.output_valve_flow_speed;
    int water_boiling_rate = system_settings.water_boiling_rate;
    int sensor_reading_timer = system_settings.sensor_reading_timer;
    int water_tank_water_max_level = system_settings.water_tank_water_max_level;
    int water_tank_water_min_level = system_settings.water_tank_water_min_level;
    int boiling_tank_water_max_level = system_settings.boiling_tank_water_max_level;
    int boiling_tank_water_min_level = system_settings.boiling_tank_water_min_level;
}