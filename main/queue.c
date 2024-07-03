#include "freertos/queue.h"
#include "types.h"

#define QUEUES_MAX_SIZE 5

static QueueHandle_t recv_packet_queue = NULL;
static QueueHandle_t sensor_readings_queue = NULL;

void initialize_queues(void){
    recv_packet_queue = xQueueCreate(QUEUES_MAX_SIZE, sizeof(data_packet_t));
    sensor_readings_queue = xQueueCreate(QUEUES_MAX_SIZE, sizeof(system_params_t));
}

uint8_t isQueueFull(QueueHandle_t queue) {
    UBaseType_t spacesAvailable = uxQueueSpacesAvailable(queue);
    return spacesAvailable == 0;
}