#ifndef STATUS_SOCKET_H
#define STATUS_SOCKET_H

#include "types.h"
#include "esp_log.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "utils.h"
#include "system_simulation.h"

#define HOST_IP_ADDR CONFIG_IPV4_ADDR
#define STATUS_PORT CONFIG_STATUS_PORT
#define MAX_JSON_LENGTH 611

// External function to set system parameters
extern void set_system_parameters(system_params_t system_settings);

// Function to send a data packet
void send_packet(data_packet_t data_packet);

// Function to set up the TCP socket
void setup_status_socket(void);

// Function to send the sensor readings a data packet
void sendSystemStatusDataPacket(sensor_readings_t sensor_readings, system_params_t system_params);

#endif // STATUS_SOCKET_H
