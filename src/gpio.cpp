#include "gpio.h"
#include "utils.h"

void Export(GPIO_Pin& pin) {
    write_to_file(export_filepath(pin.pinNumber), std::to_string(pin.pinNumber));

    std::string directionStr = "in";
    if (pin.direction == GPIO_PinDirection::WRITE) {
        directionStr = "out";
    }
    write_to_file(direction_filepath(pin.pinNumber), directionStr);
    
    pin.status = EXPORTED;
}

void Unexport(GPIO_Pin& pin) {
    write_to_file(unexport_filepath(pin.pinNumber), std::to_string(pin.pinNumber));
    pin.status = UNEXPORTED;
}

void Write(const GPIO_Pin& pin, GPIO_PinValue value) {
    if (pin.status == GPIO_PinStatus::EXPORTED && 
        pin.direction == GPIO_PinDirection::WRITE) {
        write_to_file(value_filepath(pin.pinNumber), std::to_string(value));
    } else {
        printf("Error: Could not write to GPIO value file");
    }
}

GPIO_PinValue Read(const GPIO_Pin& pin) {
    if (pin.status == GPIO_PinStatus::EXPORTED && 
        pin.direction == GPIO_PinDirection::READ) {
        
        std::string readFile = value_filepath(pin.pinNumber);
        printf("reading file: %s\n", readFile.c_str());
        std::string contents = read_entire_file(readFile.c_str());

        GPIO_PinValue val = LOW;

        if (contents == "1") {
            val = HIGH;
        }
        else if (contents == "0") {

        }
        else {
            printf("unexpected file contents %s\n", contents.c_str());
        }

        //uint8_t val = read_first_byte_of_file(value_filepath(pin.pinNumber));
        printf("Pin %d value: %d\n", pin.pinNumber, val); 

        return static_cast<GPIO_PinValue>(val);
    } else {
        printf("Error: Could not read from GPIO value file");
        return LOW;
    }
}

std::string export_filepath(uint32_t pinNumber) {
    return GPIO_FILE_PATH + "export";
}

std::string unexport_filepath(uint32_t pinNumber) {
    return GPIO_FILE_PATH + "unexport";
}

std::string direction_filepath(uint32_t pinNumber) {
    return GPIO_FILE_PATH + "gpio" + std::to_string(pinNumber) + "/direction";
}

std::string value_filepath(uint32_t pinNumber) {
    return GPIO_FILE_PATH + "gpio" + std::to_string(pinNumber) + "/value";
}


/* int main() {

} */