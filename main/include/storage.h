#pragma once

#include <stdint.h>

#define NVS_NAMESPACE "storage"
#define NVS_KEY_TARE  "hx711_offset"

/**
 * @function storage_init
 * @description Initializes the NVS flash partition.
 */
void storage_init(void);

/**
 * @function storage_load_tare
 * @description Loads the saved tare offset from NVS.
 * @param {int32_t*} offset Pointer to store the retrieved offset value.
 * @return {bool} True if the offset was successfully loaded, false otherwise.
 */
bool storage_load_tare(int32_t *offset);

/**
 * @function storage_save_tare
 * @description Saves the tare offset value to NVS.
 * @param {int32_t} offset The offset value to store.
 */
void storage_save_tare(int32_t offset);