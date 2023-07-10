#include "audio_player.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

int play(fs::path audioPath)
{
    if (!fs::exists(audioPath))
    {
        printf("audio file does not exist at %s\n", audioPath.c_str());
    }

    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    result = ma_decoder_init_file(audioPath.c_str(), NULL, &decoder);
    if (result != MA_SUCCESS) {
        printf("Could not load file: %s\n", audioPath.c_str());
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

    sleep(5);

    if (ma_device_stop(&device) != MA_SUCCESS) {
        printf("Failed to stop playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -5;
    }

    sleep(5);
    
    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -6;
    }

    sleep(5);

    if (ma_device_stop(&device) != MA_SUCCESS) {
        printf("Failed to stop playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -5;
    }

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    return 0;
}