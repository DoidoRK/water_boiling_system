#include "queue_controller.h"


void initialize_queues(void) {
    recv_packet_queue = xQueueCreate(QUEUES_MAX_SIZE, sizeof(data_packet_t));
    sensor_readings_queue = xQueueCreate(QUEUES_MAX_SIZE, sizeof(system_params_t));
}

int isQueueFull(QueueHandle_t queue) {
    UBaseType_t spacesAvailable = uxQueueSpacesAvailable(queue);
    return spacesAvailable == 0;
}