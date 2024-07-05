#ifndef SYSTEM_SIMULATION_H
#define SYSTEM_SIMULATION_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "types.h"
#include "status_socket.h"

// Function declarations
void startup_system(void);
void shutdown_system(void);
void set_system_parameters(system_params_t system_settings);

#endif // SYSTEM_SIMULATION_H
