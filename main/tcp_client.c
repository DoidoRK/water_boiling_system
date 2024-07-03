#include "tcp_client.h"

// Define the static variables
static const char *TAG = "TCP CLIENT";
struct sockaddr_in dest_addr;
int sock;
char rx_buffer[128];
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

void send_packet(data_packet_t data_packet) {
    // Convert struct to JSON
    char *payload = data_packet_to_json(data_packet);

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

void sendSystemStatysDataPacket(sensor_readings_t sensor_readings){
    data_packet_t data_packet;
    data_packet.message_type = SYSTEM_STATUS;
    data_packet.sensor_readings = sensor_readings;
    send_packet(data_packet);
}

// static void handleServerMessageTask(void* arg){
//     while (1) {
//         //WIP
//     }
// }

void setup_tcp_socket(void){
    inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return; // Add return statement to prevent further execution
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(sock); // Add cleanup on failure
        return; // Add return statement to prevent further execution
    }
    ESP_LOGI(TAG, "Successfully connected to server");
}