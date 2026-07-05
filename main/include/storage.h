#pragma once

#include <stdint.h>

#define NVS_NAMESPACE  "storage"
#define NVS_KEY_TARE   "hx711_offset"

/**
 * @brief Initializes the NVS flash partition and handles automated defragmentation/recovery.
 */
void storage_init(void);

/**
 * @brief Loads the runtime calibration offset from the non-volatile storage block.
 * @param offset Destination pointer for the retrieved offset integer.
 * @return True if key lookup was successful and data fetched, false otherwise.
 */
bool storage_load_tare(int32_t *offset);

/**
 * @brief Commits the new runtime calibration offset to the non-volatile flash storage block.
 * @param offset The offset integer payload to persist.
 */
void storage_save_tare(int32_t offset);