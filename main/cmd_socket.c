#include "cmd_socket.h"

// Define the static variables
static const char *TAG = "TCP CMD SOCKET";
struct sockaddr_in cmd_dest_addr;
int cmd_socket;

static void handleServerMessageTask(void* arg){
    ESP_LOGI(TAG, "Handle server messages task started");

    while (1) {
        char json_str[MAX_JSON_LENGTH];  // Allocate memory for JSON string

        int len = recv(cmd_socket, json_str, sizeof(json_str) - 1, 0);
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

void setup_cmd_socket(void){
    inet_pton(AF_INET, CONFIG_IPV4_ADDR, &cmd_dest_addr.sin_addr);
    cmd_dest_addr.sin_family = AF_INET;
    cmd_dest_addr.sin_port = htons(CONFIG_CMD_PORT);

    cmd_socket =  socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (cmd_socket < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return; // Add return statement to prevent further execution
    }
    ESP_LOGI(TAG, "created, connecting to %s:%d", CONFIG_IPV4_ADDR, CONFIG_CMD_PORT);

    int err = connect(cmd_socket, (struct sockaddr *)&cmd_dest_addr, sizeof(cmd_dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "unable to connect: errno %d", errno);
        close(cmd_socket); // Add cleanup on failure
        return; // Add return statement to prevent further execution
    }
    ESP_LOGI(TAG, "Successfully connected to server");

    xTaskCreate(handleServerMessageTask,"handleServerMessageTask",4096,NULL,4,NULL);
}