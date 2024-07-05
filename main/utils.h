#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include "cJSON.h"

char* data_packet_to_json(data_packet_t packet);

data_packet_t json_to_data_packet(const char *json_str);


#endif // UTILS_H