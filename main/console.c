#include "include/console.h"

#include "hal/usb_serial_jtag_ll.h"
#include "soc/interrupts.h"

SemaphoreHandle_t tare_sem = NULL;

/**
 * @function jtag_isr_handler
 * @description Low-level ISR to capture 'T' from USB FIFO and give the sync semaphore.
 */
static void IRAM_ATTR jtag_isr_handler(void *arg) {
    uint8_t rx_byte;
    bool action = false;

    while (usb_serial_jtag_ll_read_rxfifo(&rx_byte, 1) > 0)
        if (rx_byte == 'T') action = true;

    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);

    if (!action || !tare_sem) return;

    BaseType_t wakeup = pdFALSE;
    xSemaphoreGiveFromISR(tare_sem, &wakeup);

    if (wakeup) portYIELD_FROM_ISR();
}

/**
 * @function console_init
 * @description Initializes the USB serial console task for handling software commands.
 */
void console_init(void) {
    tare_sem = xSemaphoreCreateBinary();
    esp_intr_alloc(ETS_USB_SERIAL_JTAG_INTR_SOURCE, ESP_INTR_FLAG_IRAM, jtag_isr_handler, NULL, NULL);
}