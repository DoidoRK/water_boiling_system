#ifndef SYSTEM_SIMULATION_H
#define SYSTEM_SIMULATION_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "types.h"

// Sensor readings queue
extern QueueHandle_t sensor_readings_queue;

// Function declarations
void startup_system(void);
void set_system_parameters(system_params_t system_settings);

#endif // SYSTEM_SIMULATION_H
