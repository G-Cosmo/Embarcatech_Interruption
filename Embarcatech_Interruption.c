#include <stdio.h>
#include "pico/stdlib.h"

const uint ledPin_Red = 12;
#define times_per_second 5


void led_blink(uint led_pin, uint times)
{   
        gpio_put(led_pin, true);
        sleep_ms((1000/times)/2);
        gpio_put(led_pin, false);
        sleep_ms((1000/times)/2);
}

int main()
{   
    gpio_init(ledPin_Red);
    gpio_set_dir(ledPin_Red, GPIO_OUT);
    stdio_init_all();

    while (true) {
        led_blink(ledPin_Red, times_per_second);
    }
}
