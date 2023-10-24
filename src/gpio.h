#pragma once

#include <stdint.h>
#include <string>

enum GPIO_PinValue {
    LOW = 0,
    HIGH = 1
};

enum GPIO_PinDirection {
    READ,
    WRITE
};

enum GPIO_PinStatus {
    UNEXPORTED,
    EXPORTED
};

struct GPIO_Pin {
    GPIO_PinDirection direction = READ;
    GPIO_PinStatus status = UNEXPORTED;
    uint32_t pinNumber = 0;
};

void Export(GPIO_Pin& pin);
void Unexport(GPIO_Pin& pin);
void Write(const GPIO_Pin& pin, GPIO_PinValue value);
GPIO_PinValue Read(const GPIO_Pin& pin);

std::string direction_filepath(uint32_t pinNumber);
std::string export_filepath(uint32_t pinNumber);
std::string unexport_filepath(uint32_t pinNumber);
std::string value_filepath(uint32_t pinNumber);

const std::string GPIO_FILE_PATH = "/sys/class/gpio/";
