#include "status_socket.h"

// Define the static variables
static const char *TAG = "TCP SATUS SOCKET";
struct sockaddr_in dest_addr;
int status_socket;
char host_ip[] = HOST_IP_ADDR;
int addr_family = 0;
int ip_protocol = 0;

void send_packet(data_packet_t data_packet) {
    // Convert struct to JSON
    char *payload = data_packet_to_json(data_packet);
    char rx_buffer[128];

    // Send JSON payload
    int err = send(status_socket, payload, strlen(payload), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }

    int len = recv(status_socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
    // Error occurred during receiving
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
    }

    // Free the JSON string
    free(payload);
}

void sendSystemStatusDataPacket(sensor_readings_t sensor_readings, system_params_t system_params){
    data_packet_t data_packet;
    data_packet.device_type = ESP;
    data_packet.message_type = SYSTEM_STATUS;
    data_packet.system_settings = system_params;
    data_packet.sensor_readings = sensor_readings;
    send_packet(data_packet);
}

static void handleServerMessageTask(void* arg){
    ESP_LOGI(TAG, "Handle server messages task started");

    while (1) {
        char json_str[MAX_JSON_LENGTH];  // Allocate memory for JSON string

        int len = recv(status_socket, json_str, sizeof(json_str) - 1, 0);
        if (len < 0) { // Error occurred during receiving
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        }
        else { // Data received
            data_packet_t recv_pckt = json_to_data_packet(json_str);
            switch (recv_pckt.message_type)
            {
            case SYSTEM_STARTUP:
                startup_system();
                break;
            
            case SYSTEM_PARAM_CHANGE:
                set_system_parameters(recv_pckt.system_settings);
                break;

            case SYSTEM_SHUTDOWN:
                shutdown_system();
                break;

            case SYSTEM_INTR:
                //Implement INTR proccess
                break;

            default:
                ESP_LOGE(TAG, "Unknown message type received: %d", recv_pckt.message_type);
                break;
            }
        }
    }
}

void setup_status_socket(void){
    inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(STATUS_PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    status_socket =  socket(addr_family, SOCK_STREAM, ip_protocol);
    if (status_socket < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return; // Add return statement to prevent further execution
    }
    ESP_LOGI(TAG, "created, connecting to %s:%d", host_ip, STATUS_PORT);

    int err = connect(status_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "unable to connect: errno %d", errno);
        close(status_socket); // Add cleanup on failure
        return; // Add return statement to prevent further execution
    }
    ESP_LOGI(TAG, "Successfully connected to server");

    xTaskCreate(handleServerMessageTask,"handleServerMessageTask",4096,NULL,4,NULL);
}