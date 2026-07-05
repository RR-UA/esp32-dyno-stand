#pragma once

#include <stdint.h>

#define FILTER_SIZE   4
#define BLOCK_SAMPLES 8

#define HX711_DOT_PIN GPIO_NUM_3
#define HX711_SCK_PIN GPIO_NUM_4

/**
 * @brief Initializes GPIO pins and structures for synchronous HX711 communication.
 */
void hx711_init(void);

/**
 * @brief Measures the zero-point baseline, fills the filter pipeline, and returns the offset.
 * @return Calculated zero-point offset value.
 */
int32_t hx711_tare(void);

/**
 * @brief Manually overrides the internal zero-point offset and primes the filter buffer.
 * @param offset Target zero-point offset value.
 */
void hx711_set_offset(int32_t offset);

/**
 * @brief Processes cascaded filtering using block averaging and a moving window.
 * @return Fully calibrated stable weight in grams.
 */
int hx711_read_weight(void);