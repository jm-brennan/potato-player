#include "audio_player.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace fs = std::filesystem;

ma_decoder_config decoderConfig;
std::vector<fs::path> paths;
uint pathsIndex = 0;
ma_decoder* activeDecoder;
ma_decoder* nextDecoder;

static ma_data_source* next_callback(ma_data_source* pDataSource)
{
    printf("finished data source at index %d\n", pathsIndex);
    pathsIndex = (pathsIndex + 1) % paths.size();

    ma_decoder_uninit(activeDecoder);
    free(activeDecoder);
    activeDecoder = nextDecoder;
    nextDecoder = (ma_decoder*)malloc(sizeof(ma_decoder));

    uint pathIndexOfNewNext = (pathsIndex+1)%paths.size();

    ma_result result = ma_decoder_init_file(paths[pathIndexOfNewNext].c_str(), &decoderConfig, nextDecoder);
    if (result != MA_SUCCESS) {
        printf("could not init decoder for %s\n", paths[pathIndexOfNewNext].c_str());
    }
    ma_data_source_set_next_callback(nextDecoder, next_callback);

    /*
    This will be fired when the last item in the chain has reached the end. In this example we want
    to loop back to the start, so we need only return a pointer back to the head.
    */
    return activeDecoder;
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    //printf("data callback called with %d frames\n", frameCount);

    ma_uint64 cursor;

    ma_result result = ma_data_source_get_cursor_in_pcm_frames(pDevice->pUserData, &cursor);
    if (result != MA_SUCCESS) {
        return;  // Failed to retrieve the cursor.
    }

    //printf("cursor currently at frame %d\n", cursor);

    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

/* void uninit_decoders(ma_decoder* decoders, uint numDecoders) {
    for (uint i = 0; i < numDecoders; ++i) {
        ma_decoder_uninit(&decoders[i]);
    }
    free(decoders);
} */

int play(std::vector<fs::path> audioPaths)
{

    if (audioPaths.size() < 2) {
        printf("need more than 1 paths\n");
        return 1;
    }

    // TODO this is hacky and not safe yet
    paths = audioPaths;

    ma_result result;
    ma_device_config deviceConfig;
    ma_device device;

    const ma_format SAMPLE_FORMAT = (ma_format)5;
    const uint CHANNEL_COUNT = 2;
    const uint SAMPLE_RATE = 44100;

    decoderConfig = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE);
    
    activeDecoder = (ma_decoder*)malloc(sizeof(ma_decoder));
    nextDecoder = (ma_decoder*)malloc(sizeof(ma_decoder));

    result = ma_decoder_init_file(audioPaths[0].c_str(), &decoderConfig, activeDecoder);
    if (result != MA_SUCCESS) {
        printf("could not init decoder for %s\n", audioPaths[0].c_str());
    }
    ma_data_source_set_next_callback(activeDecoder, next_callback);

    result = ma_decoder_init_file(audioPaths[1].c_str(), &decoderConfig, nextDecoder);
    if (result != MA_SUCCESS) {
        printf("could not init decoder for %s\n", audioPaths[1].c_str());
    }

    ma_data_source_set_next_callback(nextDecoder, next_callback);

/* 
    for (uint i = 0; i < audioPaths.size(); ++i) {
        if (!fs::exists(audioPaths[i]))
        {
            printf("audio file does not exist at %s\n", audioPaths[i].c_str());
            continue;
        }
        result = ma_decoder_init_file(audioPaths[i].c_str(), &decoderConfig, &decoders[i]);
        if (result != MA_SUCCESS) {
            printf("Could not load file: %s\n", audioPaths[i].c_str());
            return -2;
        }

        printf("file %s outputFormat %ld\n", audioPaths[i].c_str(), decoders[i].outputFormat);
        printf("file %s outputChannels %d\n", audioPaths[i].c_str(), decoders[i].outputChannels);
        printf("file %s outputSampleRate %d\n", audioPaths[i].c_str(), decoders[i].outputSampleRate);


    } */

    /* for (uint i = 0; i < numDecoders-1; ++i) {
        if (ma_data_source_set_next(&decoders[i], &decoders[i+1])) {
            printf("bad setting next\n");
            return -11;
        }

    } */

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = SAMPLE_FORMAT;
    deviceConfig.playback.channels = CHANNEL_COUNT;
    deviceConfig.sampleRate        = SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = activeDecoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        //uninit_decoders(decoders, numDecoders);
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        //uninit_decoders(decoders, numDecoders);
        ma_device_uninit(&device);
        return -4;
    }

   /*  sleep(5);

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
    } */

    

    sleep(80);

    ma_device_uninit(&device);
    //uninit_decoders(decoders, numDecoders);
    
    free(activeDecoder);
    free(nextDecoder);

    return 0;
}