#ifndef QUEUE_H
#define QUEUE_H

#include "freertos/queue.h"
#include "types.h"

#define QUEUES_MAX_SIZE 5

// Queue handles for packet reception and sensor readings
extern QueueHandle_t recv_packet_queue;
extern QueueHandle_t sensor_readings_queue;

// Function to initialize the queues
void initialize_queues(void);

// Function to check if a queue is full
uint8_t isQueueFull(QueueHandle_t queue);

#endif // QUEUE_MANAGER_H
