#ifndef PIKEYD_GPIO_BACKEND_H
#define PIKEYD_GPIO_BACKEND_H
#include <stdint.h>
#define CLK 23
#define PL 24
#define DIN 22
#define HIGH 1
#define LOW 0
int pi_gpio_init(void);
void pi_gpio_close(void);
int pi_gpio_write(unsigned int offset, int value);
int pi_gpio_read(unsigned int offset);
void pi_gpio_delay_us(unsigned int us);
#endif
