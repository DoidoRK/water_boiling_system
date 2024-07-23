#include"utils.h"

char* sensor_readings_to_json(sensor_readings_t sensor_readings) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "draining_system", sensor_readings.draining_system);
    cJSON_AddNumberToObject(root, "max_sensor_tank1", sensor_readings.max_sensor_tank1);
    cJSON_AddNumberToObject(root, "min_sensor_tank1", sensor_readings.min_sensor_tank1);
    cJSON_AddNumberToObject(root, "water_level_tank1", sensor_readings.water_level_tank1);
    cJSON_AddNumberToObject(root, "temp_water_tank2", sensor_readings.temp_water_tank2);
    cJSON_AddNumberToObject(root, "max_sensor_tank2", sensor_readings.max_sensor_tank2);
    cJSON_AddNumberToObject(root, "min_sensor_tank2", sensor_readings.min_sensor_tank2);
    cJSON_AddNumberToObject(root, "water_level_tank2", sensor_readings.water_level_tank2);
    cJSON_AddNumberToObject(root, "input_valve_status", sensor_readings.input_valve_status);
    cJSON_AddNumberToObject(root, "middle_valve_status", sensor_readings.middle_valve_status);
    cJSON_AddNumberToObject(root, "output_valve_status", sensor_readings.output_valve_status);
    cJSON_AddNumberToObject(root, "resistance_status", sensor_readings.resistance_status);
    cJSON_AddNumberToObject(root, "water_is_boiled", sensor_readings.water_is_boiled);
    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    return json_str;
}

// Function to parse JSON string and convert to data packet structure
command_data_packet_t json_to_command_data_packet(const char *json_str) {
    command_data_packet_t packet = {0};
    // Parse JSON string
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        // Handle parsing error
        // You can add error handling code as per your application's requirement
        return packet; // Return an empty command_data_packet_t on error
    }
    // Extract message_type
    cJSON *msg_type = cJSON_GetObjectItem(root, "message_type");
    if (cJSON_IsNumber(msg_type)) {
        packet.message_type = msg_type->valueint;
    } else {
        // Handle error if "message_type" is not present or not a number
        cJSON_Delete(root);
        return packet; // Return an empty command_data_packet_t on error
    }

    // Extract system_settings
    cJSON *system_settings = cJSON_GetObjectItem(root, "system_settings");
    if (cJSON_IsObject(system_settings)) {
        packet.system_settings.input_valve_flow_speed = cJSON_GetObjectItem(system_settings, "water_supply_volume")->valueint;
        packet.system_settings.middle_valve_flow_speed = cJSON_GetObjectItem(system_settings, "boiling_tank_volume")->valueint;
        packet.system_settings.input_valve_flow_speed = cJSON_GetObjectItem(system_settings, "input_valve_flow_speed")->valueint;
        packet.system_settings.middle_valve_flow_speed = cJSON_GetObjectItem(system_settings, "middle_valve_flow_speed")->valueint;
        packet.system_settings.output_valve_flow_speed = cJSON_GetObjectItem(system_settings, "output_valve_flow_speed")->valueint;
        packet.system_settings.target_temperature = cJSON_GetObjectItem(system_settings, "target_temperature")->valueint;
        packet.system_settings.water_boiling_rate = cJSON_GetObjectItem(system_settings, "water_boiling_rate")->valueint;
        packet.system_settings.sensor_reading_timer = cJSON_GetObjectItem(system_settings, "sensor_reading_timer")->valueint;
        packet.system_settings.water_tank_water_max_level = cJSON_GetObjectItem(system_settings, "water_tank_water_max_level")->valueint;
        packet.system_settings.water_tank_water_min_level = cJSON_GetObjectItem(system_settings, "water_tank_water_min_level")->valueint;
        packet.system_settings.boiling_tank_water_max_level = cJSON_GetObjectItem(system_settings, "boiling_tank_water_max_level")->valueint;
        packet.system_settings.boiling_tank_water_min_level = cJSON_GetObjectItem(system_settings, "boiling_tank_water_min_level")->valueint;
    } else {
        // Handle error if "system_settings" is not present or not an object
        cJSON_Delete(root);
        return packet; // Return an empty command_data_packet_t on error
    }

    // Clean up cJSON resources
    cJSON_Delete(root);

    return packet;
}