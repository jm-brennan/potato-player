#include "gpio.h"
#include <unistd.h>


void read_pin() {
    GPIO_Pin pinToRead;
    pinToRead.pinNumber = 81;
    pinToRead.direction = READ;

    Export(pinToRead);

    for (uint i = 0; i < 10; ++i) {
        GPIO_PinValue val = Read(pinToRead);
        if (val == HIGH) {
            printf("value was high\n");
        }
        else {
            printf("value was low\n");
        }
        sleep(1);
    }

    Unexport(pinToRead);
}

int main() {
    read_pin();
}