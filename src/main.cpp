#include <iostream>
#include <fstream>
#include <string>
#include "gpio.h"
#include "utils.h"
#include <unistd.h>

int main() {
    printf("Initializing pin 484\n");

    GPIO_Pin pin;
    pin.direction = READ;
    pin.pinNumber = 484;

    printf("exporting\n");
    Export(pin);
    printf("exported\n");

    for (int i = 0; i < 10; ++i) {
        printf("Reading value...");
        GPIO_PinValue val = Read(pin);
        printf("\n%d\n", val);
        sleep(1);
    }

    printf("completed, unexporting\n");
    Unexport(pin);
}