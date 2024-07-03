#ifndef QUEUE_CONTROLLER_H
#define QUEUE_CONTROLLER_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "types.h"

#define QUEUES_MAX_SIZE 5

// Queue handles for packet reception and sensor readings
static QueueHandle_t recv_packet_queue;
static QueueHandle_t sensor_readings_queue;

// Function to initialize the queues
void initialize_queues(void);

// Function to check if a queue is full
int isQueueFull(QueueHandle_t queue);

#endif // QUEUE_CONTROLLER_H
