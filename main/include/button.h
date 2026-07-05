#pragma once

#define BUTTON_PIN  GPIO_NUM_9
#define DEBOUNCE_US 200000

/**
 * @brief Initializes the button GPIO with a falling edge interrupt and binds the ISR.
 */
void button_init(void);