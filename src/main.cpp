#include <iostream>
#include <fstream>
#include <string>
#include "gpio.h"
#include "utils.h"
#include <unistd.h>

int main() {
    std::cout << "Initializing pin 484";

    GPIO_Pin pin;
    pin.direction = READ;
    pin.pinNumber = 484;

    Export(pin);

    for (int i = 0; i < 10; ++i) {
        std::cout << "Pin value: " << Read(pin);
        sleep(1);
    }

    Unexport(pin);
}