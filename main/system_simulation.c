#include "system_simulation.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <math.h>

#define DELAY_CALC_RATE 150000

system_params_t current_system_params = {
    .water_supply_volume = 100,
    .boiling_tank_volume = 100,
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

int input_valve_delay = 150;
int middle_valve_delay = 150;
int output_valve_delay = 150;
int water_boiling_delay = 150;
int max_sensor_tank1 = 0, min_sensor_tank1 = 0;
int water_level_tank1 = 0;
int temp_water_tank2 = 27;
int max_sensor_tank2 = 0, min_sensor_tank2 = 0;
int water_level_tank2 = 0;
int input_valve_status = 0, middle_valve_status = 0, output_valve_status = 0, resistance_status = 0;
int water_is_boiled = 0;
int draining_system = 0;

sensor_readings_t sensor_readings;

SemaphoreHandle_t water_tank1_mutex, water_tank2_mutex, temp_water2_mutex, sensor_readings_mutex;

TaskHandle_t xSystemControlTaskHandle = NULL;
TaskHandle_t xStatusCommTaskHandle = NULL;
TaskHandle_t xInputValveControlTaskHandle = NULL;
TaskHandle_t xMiddleValveControlTaskHandle = NULL;
TaskHandle_t xResistanceControlTaskHandle = NULL;
TaskHandle_t xOutputValveControlTaskHandle = NULL;

int calculateDelay(int value){
    double temp = (1.0 / value) * DELAY_CALC_RATE;
    int time_delay = (int)ceil(temp);
    return time_delay;
}

void updateSensorReadings(){
    xSemaphoreTake(sensor_readings_mutex, portMAX_DELAY);
    sensor_readings.draining_system = draining_system;
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
    xSemaphoreGive(sensor_readings_mutex);
}

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
            if(water_level_tank1 < current_system_params.water_supply_volume){
                xSemaphoreTake(water_tank1_mutex, portMAX_DELAY);
                water_level_tank1 += 2;
                xSemaphoreGive(water_tank1_mutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(input_valve_delay));
    }
}

void MiddleValveControlTask(){
    for(;;){
        if(middle_valve_status){
            if (water_level_tank1 > 0)
            {
                xSemaphoreTake(water_tank1_mutex, portMAX_DELAY);
                water_level_tank1--;
                xSemaphoreGive(water_tank1_mutex);
            }

            if (water_level_tank2 < current_system_params.boiling_tank_volume)
            {
                xSemaphoreTake(water_tank2_mutex, portMAX_DELAY);
                water_level_tank2++;
                xSemaphoreGive(water_tank2_mutex);
            }

            if(27 < temp_water_tank2){
                xSemaphoreTake(temp_water2_mutex, portMAX_DELAY);
                temp_water_tank2--;
                xSemaphoreGive(temp_water2_mutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(middle_valve_delay));
    }
}

void ResistanceControlTask(){
    for(;;) {
        if(resistance_status){
            xSemaphoreTake(temp_water2_mutex, portMAX_DELAY);
            temp_water_tank2++;
            xSemaphoreGive(temp_water2_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(water_boiling_delay));
    }
}

void OutputValveControlTask(){
    for(;;) {
        if(output_valve_status){
            if(water_level_tank2 > 0){
                xSemaphoreTake(water_tank2_mutex, portMAX_DELAY);
                water_level_tank2--;
                xSemaphoreGive(water_tank2_mutex);
            }
        } 
        vTaskDelay(pdMS_TO_TICKS(output_valve_delay));
    }
}

void SystemControlTask(){
    for(;;){
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
        updateSensorReadings();

        vTaskDelay(pdMS_TO_TICKS(current_system_params.sensor_reading_timer));
    }
}

void StatusCommTask(){
    while (1)
    {
        xSemaphoreTake(sensor_readings_mutex, portMAX_DELAY);
        send_sensor_readings(sensor_readings);
        xSemaphoreGive(sensor_readings_mutex);
        vTaskDelay(pdMS_TO_TICKS(current_system_params.sensor_reading_timer));
    }
}

void DrainOutSystemTask(){
    ESP_LOGI(TAG, "Drain Out task started.");
    draining_system = 1;
    while (draining_system)
    {
        input_valve_status = 0;
        middle_valve_status = 0;
        output_valve_status = 0;

        readDataFromSensors();

        if(resistance_status == 1){
            resistance_status = 0;
        }
        if(sensor_readings.temp_water_tank2 > 27){
            if(sensor_readings.min_sensor_tank1 == 1){
                middle_valve_status = 1;
                output_valve_status = 1;
                input_valve_status = 0;
            } else {
                input_valve_status = 1;
            }
        } else {
            if(sensor_readings.min_sensor_tank1 == 1){
                middle_valve_status = 1;
            }
            if(sensor_readings.min_sensor_tank2 == 1){
                output_valve_status = 1;
            }
            if(sensor_readings.min_sensor_tank1 == 0 && sensor_readings.min_sensor_tank2 == 0){
                draining_system = 0;
            }
        }
        updateSensorReadings();
        vTaskDelay(pdMS_TO_TICKS(current_system_params.sensor_reading_timer));
    }
    ESP_LOGI(TAG, "System is drained, ending control tasks.");
    resistance_status = 0;
    input_valve_status = 0;
    middle_valve_status = 0;
    output_valve_status = 0;
    draining_system = 0;
    updateSensorReadings();
    vTaskDelay(pdMS_TO_TICKS(current_system_params.sensor_reading_timer));
    if( xInputValveControlTaskHandle != NULL )
    {
        vTaskDelete( xInputValveControlTaskHandle );
        xInputValveControlTaskHandle = NULL;
        ESP_LOGI(TAG, "Input Valve Task deleted.");
    }
    if( xMiddleValveControlTaskHandle != NULL )
    {
        vTaskDelete( xMiddleValveControlTaskHandle );
        xMiddleValveControlTaskHandle = NULL;
        ESP_LOGI(TAG, "Middle Valve Task deleted.");
    }
    if( xOutputValveControlTaskHandle != NULL )
    {
        vTaskDelete( xOutputValveControlTaskHandle );
        xOutputValveControlTaskHandle = NULL;
        ESP_LOGI(TAG, "Output Valve Task deleted.");
    }
    if( xResistanceControlTaskHandle != NULL )
    {
        vTaskDelete( xResistanceControlTaskHandle );
        xResistanceControlTaskHandle = NULL;
        ESP_LOGI(TAG, "Resistance Task deleted.");
    }
    if( water_tank1_mutex != NULL )
    {
        vSemaphoreDelete( water_tank1_mutex );
        water_tank1_mutex = NULL;
        ESP_LOGI(TAG, "Input valve mutex deleted.");
    }
    if( water_tank2_mutex != NULL )
    {
        vSemaphoreDelete( water_tank2_mutex );
        water_tank2_mutex = NULL;
        ESP_LOGI(TAG, "Middle valve mutex deleted.");
    }
    if( temp_water2_mutex != NULL )
    {
        vSemaphoreDelete( temp_water2_mutex );
        temp_water2_mutex = NULL;
        ESP_LOGI(TAG, "Resistance mutex deleted.");
    }
    if( sensor_readings_mutex != NULL )
    {
        vSemaphoreDelete( sensor_readings_mutex );
        sensor_readings_mutex = NULL;
        ESP_LOGI(TAG, "Sensor Readings mutex created.");
    }
    if( xStatusCommTaskHandle != NULL )
    {
        vTaskDelete( xStatusCommTaskHandle );
        xStatusCommTaskHandle = NULL;
        ESP_LOGI(TAG, "Status Comms Task deleted.");
    }
    vTaskDelete( NULL );
}

void startup_system(){
    sensor_readings_mutex = xSemaphoreCreateMutex();
    if (sensor_readings_mutex != NULL) {
        ESP_LOGI(TAG, "Sensor Readings mutex created.");
    }
    xSemaphoreGive(sensor_readings_mutex);

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

    xTaskCreate(SystemControlTask,"SystemControlTask", 4096, NULL, 1, &xSystemControlTaskHandle);
    xTaskCreate(StatusCommTask,"StatusCommTask", 4096, NULL, 2, &xStatusCommTaskHandle);
    xTaskCreate(InputValveControlTask,"InputValveControlTask",2048,NULL,3,&xInputValveControlTaskHandle);
    xTaskCreate(MiddleValveControlTask,"MiddleValveControlTask",2048,NULL,3,&xMiddleValveControlTaskHandle);
    xTaskCreate(ResistanceControlTask,"ResistanceControlTask",2048,NULL,3,&xResistanceControlTaskHandle);
    xTaskCreate(OutputValveControlTask,"OutputValveControlTask",2048,NULL,4,&xOutputValveControlTaskHandle);
}

void set_system_parameters(system_params_t system_settings) {
    current_system_params = system_settings;
    input_valve_delay = calculateDelay(system_settings.input_valve_flow_speed);
    middle_valve_delay = calculateDelay(system_settings.middle_valve_flow_speed);
    output_valve_delay = calculateDelay(system_settings.output_valve_flow_speed);
    water_boiling_delay = calculateDelay(system_settings.water_boiling_rate);
    ESP_LOGI(TAG, "input_valve_delay: %d", input_valve_delay);
    ESP_LOGI(TAG, "middle_valve_delay: %d", middle_valve_delay);
    ESP_LOGI(TAG, "output_valve_delay: %d", output_valve_delay);
    ESP_LOGI(TAG, "water_boiling_delay: %d", water_boiling_delay);
    ESP_LOGI(TAG, "System parameters updated.");
}

void shutdown_system(){
    ESP_LOGI(TAG, "System shutdown.");
    if( xSystemControlTaskHandle != NULL )
    {
        vTaskDelete( xSystemControlTaskHandle );
        xSystemControlTaskHandle = NULL;
        ESP_LOGI(TAG, "System control task deleted.");
    }
    xSemaphoreGive(sensor_readings_mutex);
    xTaskCreate(DrainOutSystemTask,"DrainOutSystemTask", 4096, NULL, 1, &xSystemControlTaskHandle);
}