#include "include/console.h"

#include "hal/usb_serial_jtag_ll.h"
#include "soc/interrupts.h"

SemaphoreHandle_t tare_sem = NULL;

/**
 * @brief Low-level JTAG ISR that captures the 'T' command byte from the hardware RX FIFO.
 * @param arg Unused ISR argument context.
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
 * @brief Initializes the USB serial JTAG peripheral hardware and registers its receive interrupt.
 */
void console_init(void) {
    tare_sem = xSemaphoreCreateBinary();
    esp_intr_alloc(ETS_USB_SERIAL_JTAG_INTR_SOURCE, ESP_INTR_FLAG_IRAM, jtag_isr_handler, NULL, NULL);
    usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
}