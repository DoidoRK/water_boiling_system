#include "system_simulation.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

system_params_t current_system_params = {
    .input_valve_flow_speed = 150,
    .middle_valve_flow_speed = 150,
    .output_valve_flow_speed = 150,
    .target_temperature = 70,
    .water_boiling_rate = 150,
    .sensor_reading_timer = 100,
    .water_tank_water_max_level = 95,
    .water_tank_water_min_level = 20,
    .boiling_tank_water_max_level = 95,
    .boiling_tank_water_min_level = 20,
};

static const char *TAG = "SYSTEM_SIMULATION";

int max_sensor_tank1 = 0, min_sensor_tank1 = 0;
int water_level_tank1 = 0;
int temp_water_tank2 = 27;
int max_sensor_tank2 = 0, min_sensor_tank2 = 0;
int water_level_tank2 = 0;
int input_valve_status = 0, middle_valve_status = 0, output_valve_status = 0, resistance_status = 0;
int water_is_boiled = 0;

SemaphoreHandle_t water_tank1_mutex, water_tank2_mutex, temp_water2_mutex;

void readDataFromSensors(){
    if (current_system_params.water_tank_water_max_level <= water_level_tank1) {
        max_sensor_tank1 = 1;
    } else {
        max_sensor_tank1 = 0;
        if (current_system_params.water_tank_water_min_level <= water_level_tank1) {
            min_sensor_tank1 = 1;
        } else {
            min_sensor_tank1 = 0;
        }
    }

    if (current_system_params.boiling_tank_water_max_level <= water_level_tank2) {
        max_sensor_tank2 = 1;
    } else {
        max_sensor_tank2 = 0;
        if(current_system_params.boiling_tank_water_min_level <= water_level_tank2){
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
        vTaskDelay(pdMS_TO_TICKS(current_system_params.input_valve_flow_speed));
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
        vTaskDelay(pdMS_TO_TICKS(current_system_params.middle_valve_flow_speed));
    }
}

void ResistanceControlTask(){
    for(;;) {
        if(resistance_status){
            xSemaphoreTake(temp_water2_mutex, portMAX_DELAY);
            temp_water_tank2++;
            xSemaphoreGive(temp_water2_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(current_system_params.water_boiling_rate));
    }
}

void OutputValveControlTask(){
    for(;;) {
        if(output_valve_status){
            xSemaphoreTake(water_tank2_mutex, portMAX_DELAY);
            water_level_tank2--;
            xSemaphoreGive(water_tank2_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(current_system_params.output_valve_flow_speed));
    }
}

void SystemControlTask(){
    for(;;){
        sensor_readings_t sensor_readings;
        input_valve_status = 0;
        middle_valve_status = 0;
        output_valve_status = 0;
        water_is_boiled = 0;
        
        
        readDataFromSensors();

        if(1 != max_sensor_tank1){
            input_valve_status = 1;
        }

        if(current_system_params.target_temperature <= temp_water_tank2){
            resistance_status = 0;
            water_is_boiled = 1;
        }

        if(1 == max_sensor_tank2 && current_system_params.target_temperature > temp_water_tank2){
            resistance_status = 1;
        }

        if(!min_sensor_tank2){
            water_is_boiled = 0;
        }

        if(1 != max_sensor_tank2 && water_is_boiled != 1){
            middle_valve_status = 1;
        }

        if(current_system_params.target_temperature <= temp_water_tank2 && min_sensor_tank2 != 0){
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

        send_sensor_readings(sensor_readings);
        vTaskDelay(pdMS_TO_TICKS(current_system_params.sensor_reading_timer));
    }
}

void startup_system(){
    water_tank1_mutex = xSemaphoreCreateMutex();
    if (water_tank1_mutex != NULL) {
        ESP_LOGI(TAG, "Input valve mutex created.");
    }
    xSemaphoreGive(water_tank1_mutex);

    water_tank2_mutex = xSemaphoreCreateMutex();
    if (water_tank2_mutex != NULL) {
        ESP_LOGI(TAG, "Middle valve mutex created.");
    }
    xSemaphoreGive(water_tank2_mutex);

    temp_water2_mutex = xSemaphoreCreateMutex();
    if (temp_water2_mutex != NULL) {
        ESP_LOGI(TAG, "Resistance mutex created.");
    }
    xSemaphoreGive(temp_water2_mutex);

    xTaskCreate(SystemControlTask,"SystemControlTask", 4096, NULL, 1, NULL);
    xTaskCreate(InputValveControlTask,"InputValveControlTask",2048,NULL,3,NULL);
    xTaskCreate(MiddleValveControlTask,"MiddleValveControlTask",2048,NULL,3,NULL);
    xTaskCreate(ResistanceControlTask,"ResistanceControlTask",2048,NULL,3,NULL);
    xTaskCreate(OutputValveControlTask,"OutputValveControlTask",2048,NULL,4,NULL);
}

void set_system_parameters(system_params_t system_settings) {
    current_system_params = system_settings;
    ESP_LOGI(TAG, "System parameters updated.");
}

void shutdown_system(){
    ESP_LOGI(TAG, "System shutdown.");
    xSemaphoreTake(water_tank1_mutex, portMAX_DELAY);
    xSemaphoreTake(water_tank1_mutex, portMAX_DELAY);
    xSemaphoreTake(water_tank2_mutex, portMAX_DELAY);
    xSemaphoreTake(temp_water2_mutex, portMAX_DELAY);
    xSemaphoreTake(water_tank2_mutex, portMAX_DELAY);
    input_valve_status = 1;
    middle_valve_status = 1;
    output_valve_status = 1;
}