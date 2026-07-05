#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

// includes
#include "include/button.h"
#include "include/console.h"
#include "include/hx711.h"
#include "include/storage.h"

/**
 * @brief Application entry point. Prepares storage and HX711, handling tare persistence lifecycle.
 */
void app_main(void) {
    console_init();
    storage_init();
    hx711_init();
    button_init();

    int32_t saved_offset = 0;

    if (storage_load_tare(&saved_offset)) hx711_set_offset(saved_offset);
    else storage_save_tare(hx711_tare());

    while (1) {
        const int weight = hx711_read_weight();

        if (xSemaphoreTake(tare_sem, 0) == pdTRUE) storage_save_tare(hx711_tare());
        else printf("%d\n", weight);
    }
}