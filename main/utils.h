#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include "cJSON.h"


command_data_packet_t json_to_command_data_packet(const char *json_str);
char* sensor_readings_to_json(sensor_readings_t sensor_readings);

#endif // UTILS_H