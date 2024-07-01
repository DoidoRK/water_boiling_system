#include "types.h"
#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cJSON>
#include "esp_netif.h"
#include "esp_log.h"

#define HOST_IP_ADDR CONFIG_IPV4_ADDR
#define PORT CONFIG_PORT

static const char *TAG = "example";
static const char *payload = "Message from ESP32 ";
struct sockaddr_in dest_addr;
int sock;
char rx_buffer[128];
char host_ip[] = HOST_IP_ADDR;
int addr_family = 0;
int ip_protocol = 0;


void setup_tcp_socket(void){
    inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        }
    ESP_LOGI(TAG, "Successfully connected to server");
    int err = send(sock, payload, strlen(payload), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }

    int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    // Error occurred during receiving
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
    }
    // Data received
    else {
        rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
        ESP_LOGI(TAG, "%s", rx_buffer);
    }
}

// Function to convert system_settings_t to JSON string
char* struct_to_json(system_settings_t system_status) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "input_valve_flow_speed", system_status.input_valve_flow_speed);
    cJSON_AddNumberToObject(root, "middle_valve_flow_speed", system_status.middle_valve_flow_speed);
    cJSON_AddNumberToObject(root, "output_valve_flow_speed", system_status.output_valve_flow_speed);
    cJSON_AddNumberToObject(root, "water_boiling_rate", system_status.water_boiling_rate);
    cJSON_AddNumberToObject(root, "sensor_reading_timer", system_status.sensor_reading_timer);
    cJSON_AddNumberToObject(root, "water_tank_water_max_level", system_status.water_tank_water_max_level);
    cJSON_AddNumberToObject(root, "water_tank_water_min_level", system_status.water_tank_water_min_level);
    cJSON_AddNumberToObject(root, "boiling_tank_water_max_level", system_status.boiling_tank_water_max_level);
    cJSON_AddNumberToObject(root, "boiling_tank_water_min_level", system_status.boiling_tank_water_min_level);

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    return json_str;
}

void send_packet(system_settings_t system_status) {
    // Convert struct to JSON
    char *payload = struct_to_json(system_status);

    // Send JSON payload
    int err = send(sock, payload, strlen(payload), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }

    int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    // Error occurred during receiving
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
    }

    // Free the JSON string
    free(payload);
}