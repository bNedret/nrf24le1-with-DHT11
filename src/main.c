//nRF24LE1 Transmitter
#include "reg24le1.h"
#include "gpio.h"
#include "rf.h"
#include "delay.h"
#include "dht.h"
#include "uart.h"
#include "stdio.h"

void main()
{

    
    unsigned char datavar[5];

    //Setup LED diode pin
    gpio_pin_configure(GPIO_PIN_ID_P1_4, GPIO_PIN_CONFIG_OPTION_DIR_OUTPUT | GPIO_PIN_CONFIG_OPTION_PIN_MODE_INPUT_BUFFER_ON_PULL_UP_RESISTOR);

    rf_configure_debug_lite(false, 5);
    delay_us(130);

    while (true) {



        dht11_start();
        dht11_read_byte(&datavar[0]);
        dht11_read_byte(&datavar[1]);
        dht11_read_byte(&datavar[2]);
        dht11_read_byte(&datavar[3]);
        dht11_read_byte(&datavar[4]);
        dht11_stop();


        rf_write_tx_payload(&datavar[0], 5, true);
        while (!(rf_irq_pin_active() && rf_irq_tx_ds_active()));

        rf_irq_clear_all(); //clear all interrupts in the 24L01

        delay_ms(10);


        gpio_pin_val_complement(GPIO_PIN_ID_P1_4);
        delay_ms(5000);
    }
}