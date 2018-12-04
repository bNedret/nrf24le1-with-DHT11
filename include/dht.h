//DHT11 header file
#ifndef DHT_H
#define DHT_H

#define STATE_OK 0

int dht11_start(void);

int dht11_read_byte(unsigned char* dat);

void dht11_stop(void);



#endif

