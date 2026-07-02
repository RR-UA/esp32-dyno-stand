#include "include/hx711.h"

#include "driver/gpio.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "rom/ets_sys.h"

static int32_t filter_buf[FILTER_SIZE] = { 0 };
static int filter_idx                  = 0;

static int32_t hx711_offset = 0;
static SemaphoreHandle_t hx711_data_ready_sem;

/**
 * ISR triggered on DOT falling edge, signals that conversion is ready.
 */
static void IRAM_ATTR hx711_isr_handler(void *arg) {
    BaseType_t high_task_wakeup = pdFALSE;
    xSemaphoreGiveFromISR(hx711_data_ready_sem, &high_task_wakeup);
    if (high_task_wakeup) portYIELD_FROM_ISR();
}

/**
 * Reads one raw 24-bit conversion from HX711 (gain 128, channel A).
 * @return Sign-extended 32-bit raw ADC value.
 */
static int32_t hx711_read_raw(void) {
    xSemaphoreTake(hx711_data_ready_sem, portMAX_DELAY);
    gpio_intr_disable(HX711_DOT_PIN);
    portDISABLE_INTERRUPTS();

    int32_t value = 0;
    for (int i = 0; i < 24; i++) {
        gpio_set_level(HX711_SCK_PIN, 1);
        ets_delay_us(1);
        value = (value << 1) | gpio_get_level(HX711_DOT_PIN);
        gpio_set_level(HX711_SCK_PIN, 0);
        ets_delay_us(1);
    }

    gpio_set_level(HX711_SCK_PIN, 1);
    ets_delay_us(1);
    gpio_set_level(HX711_SCK_PIN, 0);
    ets_delay_us(1);

    portENABLE_INTERRUPTS();
    xSemaphoreTake(hx711_data_ready_sem, 0);
    gpio_intr_enable(HX711_DOT_PIN);

    if (value & 0x800000) value |= (int32_t)0xFF000000;

    return value;
}

/**
 * Averages N raw readings.
 * @param samples Number of readings to average.
 * @return Average raw ADC value.
 */
static int32_t hx711_read_avg(const int samples) {
    int32_t sum = 0;
    for (int i = 0; i < samples; i++) sum += hx711_read_raw();

    return sum / samples;
}

void hx711_init(void) {
    const gpio_config_t dout_cfg = {
        .pin_bit_mask = 1ULL << HX711_DOT_PIN,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&dout_cfg);

    const gpio_config_t sck_cfg = {
        .pin_bit_mask = 1ULL << HX711_SCK_PIN,
        .mode         = GPIO_MODE_OUTPUT,
    };
    gpio_config(&sck_cfg);
    gpio_set_level(HX711_SCK_PIN, 0);

    hx711_data_ready_sem = xSemaphoreCreateBinary();

    gpio_install_isr_service(0);
    gpio_isr_handler_add(HX711_DOT_PIN, hx711_isr_handler, NULL);
}

int32_t hx711_tare(void) {
    vTaskDelay(pdMS_TO_TICKS(500));
    hx711_offset = hx711_read_avg(FILTER_SIZE);
    
    for (int i = 0; i < FILTER_SIZE; i++) filter_buf[i] = hx711_offset;

    return hx711_offset;
}

void hx711_set_offset(const int32_t offset) {
    hx711_offset = offset;
    for (int i = 0; i < FILTER_SIZE; i++) filter_buf[i] = hx711_offset;
}

/**
 * @function hx711_read_weight
 * @description Processes cascaded filtering using block averaging and a moving window.
 * @return {int} Fully calibrated stable weight in grams.
 */
int hx711_read_weight(void) {
    filter_buf[filter_idx] = hx711_read_avg(BLOCK_SAMPLES);
    filter_idx             = (filter_idx + 1) % FILTER_SIZE;
    int32_t sum            = 0;

    for (int i = 0; i < FILTER_SIZE; i++) sum += filter_buf[i];

    const int32_t moving_avg = sum / FILTER_SIZE;
    return (int32_t)((float)(moving_avg - hx711_offset) / SCALE_FACTOR);
}