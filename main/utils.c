#include"utils.h"

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