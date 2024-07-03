#include "queue.h"

// Define the queue handles
QueueHandle_t recv_packet_queue = NULL;
QueueHandle_t sensor_readings_queue = NULL;

void initialize_queues(void) {
    recv_packet_queue = xQueueCreate(QUEUES_MAX_SIZE, sizeof(data_packet_t));
    sensor_readings_queue = xQueueCreate(QUEUES_MAX_SIZE, sizeof(system_params_t));
}

uint8_t isQueueFull(QueueHandle_t queue) {
    UBaseType_t spacesAvailable = uxQueueSpacesAvailable(queue);
    return spacesAvailable == 0;
}