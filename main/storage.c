#include "include/storage.h"

#include "nvs.h"
#include "nvs_flash.h"

void storage_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

bool storage_load_tare(int32_t *offset) {
    nvs_handle_t nvs_handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle) != ESP_OK) return false;

    const esp_err_t err = nvs_get_i32(nvs_handle, NVS_KEY_TARE, offset);
    nvs_close(nvs_handle);
    return (err == ESP_OK);
}

void storage_save_tare(const int32_t offset) {
    nvs_handle_t nvs_handle;

    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle) != ESP_OK) return;

    nvs_set_i32(nvs_handle, NVS_KEY_TARE, offset);
    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
}