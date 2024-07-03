#include "tcp_client.h"

// Define the static variables
static const char *TAG = "TCP CLIENT";
struct sockaddr_in dest_addr;
int server_socket;
char host_ip[] = HOST_IP_ADDR;
int addr_family = 0;
int ip_protocol = 0;

char* data_packet_to_json(data_packet_t packet) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "message_type", packet.message_type);

    cJSON *system_settings = cJSON_CreateObject();
    cJSON_AddNumberToObject(system_settings, "input_valve_flow_speed", packet.system_settings.input_valve_flow_speed);
    cJSON_AddNumberToObject(system_settings, "middle_valve_flow_speed", packet.system_settings.middle_valve_flow_speed);
    cJSON_AddNumberToObject(system_settings, "output_valve_flow_speed", packet.system_settings.output_valve_flow_speed);
    cJSON_AddNumberToObject(system_settings, "water_boiling_rate", packet.system_settings.water_boiling_rate);
    cJSON_AddNumberToObject(system_settings, "sensor_reading_timer", packet.system_settings.sensor_reading_timer);
    cJSON_AddNumberToObject(system_settings, "water_tank_water_max_level", packet.system_settings.water_tank_water_max_level);
    cJSON_AddNumberToObject(system_settings, "water_tank_water_min_level", packet.system_settings.water_tank_water_min_level);
    cJSON_AddNumberToObject(system_settings, "boiling_tank_water_max_level", packet.system_settings.boiling_tank_water_max_level);
    cJSON_AddNumberToObject(system_settings, "boiling_tank_water_min_level", packet.system_settings.boiling_tank_water_min_level);
    cJSON_AddItemToObject(root, "system_settings", system_settings);

    cJSON *sensor_readings = cJSON_CreateObject();
    cJSON_AddNumberToObject(sensor_readings, "max_sensor_tank1", packet.sensor_readings.max_sensor_tank1);
    cJSON_AddNumberToObject(sensor_readings, "min_sensor_tank1", packet.sensor_readings.min_sensor_tank1);
    cJSON_AddNumberToObject(sensor_readings, "water_level_tank1", packet.sensor_readings.water_level_tank1);
    cJSON_AddNumberToObject(sensor_readings, "temp_water_tank2", packet.sensor_readings.temp_water_tank2);
    cJSON_AddNumberToObject(sensor_readings, "max_sensor_tank2", packet.sensor_readings.max_sensor_tank2);
    cJSON_AddNumberToObject(sensor_readings, "min_sensor_tank2", packet.sensor_readings.min_sensor_tank2);
    cJSON_AddNumberToObject(sensor_readings, "water_level_tank2", packet.sensor_readings.water_level_tank2);
    cJSON_AddNumberToObject(sensor_readings, "input_valve_status", packet.sensor_readings.input_valve_status);
    cJSON_AddNumberToObject(sensor_readings, "middle_valve_status", packet.sensor_readings.middle_valve_status);
    cJSON_AddNumberToObject(sensor_readings, "output_valve_status", packet.sensor_readings.output_valve_status);
    cJSON_AddNumberToObject(sensor_readings, "resistance_status", packet.sensor_readings.resistance_status);
    cJSON_AddNumberToObject(sensor_readings, "water_is_boiled", packet.sensor_readings.water_is_boiled);
    cJSON_AddItemToObject(root, "sensor_readings", sensor_readings);

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    return json_str;
}

// Function to parse JSON string and convert to data packet structure
data_packet_t json_to_data_packet(const char *json_str) {
    data_packet_t packet = {0};
    
    // Parse JSON string
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        // Handle parsing error
        // You can add error handling code as per your application's requirement
        return packet; // Return an empty data_packet_t on error
    }

    // Extract message_type
    cJSON *msg_type = cJSON_GetObjectItem(root, "message_type");
    if (cJSON_IsNumber(msg_type)) {
        packet.message_type = msg_type->valueint;
    } else {
        // Handle error if "message_type" is not present or not a number
        cJSON_Delete(root);
        return packet; // Return an empty data_packet_t on error
    }

    // Extract system_settings
    cJSON *system_settings = cJSON_GetObjectItem(root, "system_settings");
    if (cJSON_IsObject(system_settings)) {
        packet.system_settings.input_valve_flow_speed = cJSON_GetObjectItem(system_settings, "input_valve_flow_speed")->valueint;
        packet.system_settings.middle_valve_flow_speed = cJSON_GetObjectItem(system_settings, "middle_valve_flow_speed")->valueint;
        packet.system_settings.output_valve_flow_speed = cJSON_GetObjectItem(system_settings, "output_valve_flow_speed")->valueint;
        packet.system_settings.water_boiling_rate = cJSON_GetObjectItem(system_settings, "water_boiling_rate")->valueint;
        packet.system_settings.sensor_reading_timer = cJSON_GetObjectItem(system_settings, "sensor_reading_timer")->valueint;
        packet.system_settings.water_tank_water_max_level = cJSON_GetObjectItem(system_settings, "water_tank_water_max_level")->valueint;
        packet.system_settings.water_tank_water_min_level = cJSON_GetObjectItem(system_settings, "water_tank_water_min_level")->valueint;
        packet.system_settings.boiling_tank_water_max_level = cJSON_GetObjectItem(system_settings, "boiling_tank_water_max_level")->valueint;
        packet.system_settings.boiling_tank_water_min_level = cJSON_GetObjectItem(system_settings, "boiling_tank_water_min_level")->valueint;
    } else {
        // Handle error if "system_settings" is not present or not an object
        cJSON_Delete(root);
        return packet; // Return an empty data_packet_t on error
    }

    // Extract sensor_readings
    cJSON *sensor_readings = cJSON_GetObjectItem(root, "sensor_readings");
    if (cJSON_IsObject(sensor_readings)) {
        packet.sensor_readings.max_sensor_tank1 = cJSON_GetObjectItem(sensor_readings, "max_sensor_tank1")->valueint;
        packet.sensor_readings.min_sensor_tank1 = cJSON_GetObjectItem(sensor_readings, "min_sensor_tank1")->valueint;
        packet.sensor_readings.water_level_tank1 = cJSON_GetObjectItem(sensor_readings, "water_level_tank1")->valueint;
        packet.sensor_readings.temp_water_tank2 = cJSON_GetObjectItem(sensor_readings, "temp_water_tank2")->valueint;
        packet.sensor_readings.max_sensor_tank2 = cJSON_GetObjectItem(sensor_readings, "max_sensor_tank2")->valueint;
        packet.sensor_readings.min_sensor_tank2 = cJSON_GetObjectItem(sensor_readings, "min_sensor_tank2")->valueint;
        packet.sensor_readings.water_level_tank2 = cJSON_GetObjectItem(sensor_readings, "water_level_tank2")->valueint;
        packet.sensor_readings.input_valve_status = cJSON_GetObjectItem(sensor_readings, "input_valve_status")->valueint;
        packet.sensor_readings.middle_valve_status = cJSON_GetObjectItem(sensor_readings, "middle_valve_status")->valueint;
        packet.sensor_readings.output_valve_status = cJSON_GetObjectItem(sensor_readings, "output_valve_status")->valueint;
        packet.sensor_readings.resistance_status = cJSON_GetObjectItem(sensor_readings, "resistance_status")->valueint;
        packet.sensor_readings.water_is_boiled = cJSON_GetObjectItem(sensor_readings, "water_is_boiled")->valueint;
    } else {
        // Handle error if "sensor_readings" is not present or not an object
        cJSON_Delete(root);
        return packet; // Return an empty data_packet_t on error
    }

    // Clean up cJSON resources
    cJSON_Delete(root);

    return packet;
}

void send_packet(data_packet_t data_packet) {
    // Convert struct to JSON
    char *payload = data_packet_to_json(data_packet);
    char rx_buffer[128];

    // Send JSON payload
    int err = send(server_socket, payload, strlen(payload), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }

    int len = recv(server_socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
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

        int len = recv(server_socket, json_str, sizeof(json_str) - 1, 0);
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

void setup_tcp_socket(void){
    inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    server_socket =  socket(addr_family, SOCK_STREAM, ip_protocol);
    if (server_socket < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return; // Add return statement to prevent further execution
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

    int err = connect(server_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(server_socket); // Add cleanup on failure
        return; // Add return statement to prevent further execution
    }
    ESP_LOGI(TAG, "Successfully connected to server");

    xTaskCreate(handleServerMessageTask,"handleServerMessageTask",4096,NULL,4,NULL);
}