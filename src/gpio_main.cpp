#include "gpio.h"
#include <unistd.h>


void read_pin_infinite() {
    GPIO_Pin pinToRead;
    pinToRead.pinNumber = 81;
    pinToRead.direction = READ;

    while (true) {
        GPIO_PinValue val = Read(pinToRead);
        if (val == HIGH) {
            printf("value was high\n");
        }
        else {
            printf("value was low\n");
        }
        sleep(1);
    }
}

int main() {
    read_pin_infinite();
}