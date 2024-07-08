#ifndef CMD_SOCKET_H
#define CMD_SOCKET_H

#include "types.h"
#include "esp_log.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "utils.h"
#include "system_simulation.h"

#define MAX_JSON_LENGTH 1024

// Function to set up the TCP socket
void setup_cmd_socket(void);

#endif // CMD_SOCKET_H