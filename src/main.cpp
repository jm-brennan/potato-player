#include <iostream>
#include <fstream>
#include <string>
#include "gpio.h"
#include "utils.h"
#include <unistd.h>
#include <filesystem>
#include <thread>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdio.h>

//#include <SFML/Audio.hpp>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

int main(int argc, char** argv) {

    //sf::Music music;

    //std::filesystem::path p ("C:/absolute/path/to/the/audio/file.wav");
    //assert(std::filesystem::exists(p));

    //music.openFromFile("/home/jacob/src/potato-mp3-player/res/July.mp3");
    //music.play();

    // Wait for the music to finish, or the program would finish before hearing any sound.
    //std::this_thread::sleep_for(std::chrono::seconds(10));
    //return 0;

    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }

    result = ma_decoder_init_file(argv[1], NULL, &decoder);
    if (result != MA_SUCCESS) {
        printf("Could not load file: %s\n", argv[1]);
        return -2;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -4;
    }

    printf("Press Enter to quit...");
    getchar();

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);


   /*  printf("Initializing pin 484\n");

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
    Unexport(pin); */
}