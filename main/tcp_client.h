#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "types.h"
#include "cJSON.h"
#include "esp_log.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define HOST_IP_ADDR CONFIG_IPV4_ADDR
#define PORT CONFIG_PORT

// External function to set system parameters
extern void set_system_parameters(system_params_t system_settings);

// Function to convert a data packet to a JSON string
char* data_packet_to_json(data_packet_t packet);

// Function to send a data packet
void send_packet(data_packet_t data_packet);

// Function to set up the TCP socket
void setup_tcp_socket(void);

#endif // TCP_CLIENT_H
