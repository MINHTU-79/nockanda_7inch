#ifndef MUTEX_H
#define MUTEX_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t xMutexFetchProduction;

#endif // MUTEX_H
