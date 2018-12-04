//DHT11 source file
#include "dht.h"
#include "reg24le1.h"
#include "gpio.h"
#include "delay.h"
#include "timer0.h"

#define DHTPIN GPIO_PIN_ID_P1_3 //define pin for DHT11
#define DHT11_INPUT  gpio_pin_configure(DHTPIN, GPIO_PIN_CONFIG_OPTION_DIR_INPUT |GPIO_PIN_CONFIG_OPTION_PIN_MODE_INPUT_BUFFER_ON_NO_RESISTORS) // configure DHTPIN as input
#define DHT11_OUTPUT gpio_pin_configure(DHTPIN, GPIO_PIN_CONFIG_OPTION_DIR_OUTPUT |GPIO_PIN_CONFIG_OPTION_PIN_MODE_INPUT_BUFFER_ON_NO_RESISTORS) // configure DHTPIN as output

#define DHT11_OUT_LOW gpio_pin_val_write(DHTPIN, false) // set DHT11 low
#define DHT11_OUT_HIGH gpio_pin_val_write(DHTPIN, true) // set DHT11 high
#define DHT11_IN_DAT gpio_pin_val_read(DHTPIN) // read data from DHT11

// timeouts for timer0
#define TIMEOUT_100US 130 
#define TIMEOUT_50US 75 
#define TIMEOUT_80US 104

int dht11_start(void)
{
       
    timer0_configure(TIMER0_CONFIG_OPTION_FUNCTION_TIMER | TIMER0_CONFIG_OPTION_MODE_2_8_BIT_AUTO_RLD_TMR, 13);
    

    DHT11_OUT_LOW;
    delay_ms(20); //drive low for 2000x10us > 18ms
    DHT11_OUT_HIGH;
    delay_us(40); //40us
    DHT11_INPUT;
    if (0 != DHT11_IN_DAT)return -100; //dht11 should resp low
    
    TL0 = 0;
    timer0_run();
    while (!DHT11_IN_DAT &&  TL0< TIMEOUT_100US);
    if (TL0 >= 100) return -101;
    
    TL0=0;
    while (DHT11_IN_DAT && TL0< TIMEOUT_100US);
    if (TL0 >= 100) return -102;
    
    timer0_stop();
    return STATE_OK;
}

int dht11_read_byte(uint8_t* dat)
{
    int i;
    uint8_t d = 0;
    uint8_t count = 0;
    
    timer0_configure(TIMER0_CONFIG_OPTION_FUNCTION_TIMER | TIMER0_CONFIG_OPTION_MODE_2_8_BIT_AUTO_RLD_TMR, 13);
    
    timer0_run();

    for (i = 0; i < 8; i++) {
        d <<= 1;
        
        TL0 = 0;
        while (!DHT11_IN_DAT && TL0< TIMEOUT_80US);
        if (TL0 >= TIMEOUT_80US) return -103;
        
        TL0=0;
        while (DHT11_IN_DAT && TL0< TIMEOUT_100US);
        if (TL0 >= TIMEOUT_100US) return -104;

        //0:high 28us; 1: high 70us
        if (TL0 < TIMEOUT_50US ) {
        }//high 26~28us means 0
        else d += 1; //high 70us means 1
    }
    timer0_stop();
    

    if (dat)*dat = d;
    return STATE_OK;
}

void dht11_stop(void)
{
    DHT11_OUTPUT;
    DHT11_OUT_HIGH;
}