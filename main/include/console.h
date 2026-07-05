#pragma once

#include "freertos/FreeRTOS.h"

extern SemaphoreHandle_t tare_sem;

/**
 * @brief Initializes the USB serial JTAG peripheral hardware and registers its receive interrupt.
 */
void console_init(void);