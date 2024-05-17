/*
Demonstrates one way to chain together a number of data sources so they play back seamlessly
without gaps.

This example uses the chaining system built into the `ma_data_source` API. It will take every sound
passed onto the command line in order, and then loop back and start again. When looping a chain of
data sources, you need only link the last data source back to the first one.

To play a chain of data sources, you first need to set up your chain. To set the data source that
should be played after another, you have two options:

    * Set a pointer to a specific data source
    * Set a callback that will fire when the next data source needs to be retrieved

The first option is good for simple scenarios. The second option is useful if you need to perform
some action when the end of a sound is reached. This example will be using both.

When reading data from a chain, you always read from the head data source. Internally miniaudio
will track a pointer to the data source in the chain that is currently playing. If you don't
consistently read from the head data source this state will become inconsistent and things won't
work correctly. When using a chain, this pointer needs to be reset if you need to play the
chain again from the start:

    ```c
    ma_data_source_set_current(&headDataSource, &headDataSource);
    ma_data_source_seek_to_pcm_frame(&headDataSource, 0);
    ```

The code above is setting the "current" data source in the chain to the head data source, thereby
starting the chain from the start again. It is also seeking the head data source back to the start
so that playback starts from the start as expected. You do not need to seek non-head items back to
the start as miniaudio will do that for you internally.
*/
#define MA_EXPERIMENTAL__DATA_LOOPING_AND_CHAINING
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <filesystem>
#include <vector>

/*
For simplicity, this example requires the device to use floating point samples.
*/
#define SAMPLE_FORMAT   ma_format_f32
#define CHANNEL_COUNT   2
#define SAMPLE_RATE     48000

namespace fs = std::filesystem;

ma_uint32   g_decoderCount;
ma_decoder* decoders;

static ma_data_source* next_callback_tail(ma_data_source* pDataSource) {
    MA_ASSERT(g_decoderCount > 0);  /* <-- We check for this in main() so should never happen. */

    /*
    This will be fired when the last item in the chain has reached the end. In this example we want
    to loop back to the start, so we need only return a pointer back to the head.
    */
    return &decoders[0];
}

static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_uint64 cursor;

    ma_result result = ma_data_source_get_cursor_in_pcm_frames(pDevice->pUserData, &cursor);
    if (result != MA_SUCCESS) {
        return;  // Failed to retrieve the cursor.
    }

    printf("cursor currently at frame %d\n", cursor);
    /*
    We can just read from the first decoder and miniaudio will resolve the chain for us. Note that
    if you want to loop the chain, like we're doing in this example, you need to set the `loop`
    parameter to false, or else only the current data source will be looped.
    */
    ma_data_source_read_pcm_frames(&decoders[0], pOutput, frameCount, NULL);

    /* Unused in this example. */
    (void)pDevice;
    (void)pInput;
}

int play(std::vector<fs::path> audioPaths, fs::path next) {
    ma_result result = MA_SUCCESS;
    ma_uint32 iDecoder;
    ma_decoder_config decoderConfig;
    ma_device_config deviceConfig;
    ma_device device;

    g_decoderCount = audioPaths.size()+1;
    decoders    = (ma_decoder*)malloc(sizeof(*decoders) * g_decoderCount);

    /* In this example, all decoders need to have the same output format. */
    decoderConfig = ma_decoder_config_init(SAMPLE_FORMAT, CHANNEL_COUNT, SAMPLE_RATE);
    for (iDecoder = 0; iDecoder < g_decoderCount-1; ++iDecoder) {
        result = ma_decoder_init_file(audioPaths[iDecoder].c_str(), &decoderConfig, &decoders[iDecoder]);
        if (result != MA_SUCCESS) {
            ma_uint32 iDecoder2;
            for (iDecoder2 = 0; iDecoder2 < iDecoder; ++iDecoder2) {
                ma_decoder_uninit(&decoders[iDecoder2]);
            }
            free(decoders);

            printf("Failed to load %s.\n", audioPaths[iDecoder].c_str());
            return -1;
        }
    }

    /*
    We're going to set up our decoders to run one after the other, but then have the last one loop back
    to the first one. For demonstration purposes we're going to use the callback method for the last
    data source.
    */
    for (iDecoder = 0; iDecoder < g_decoderCount-2; iDecoder += 1) {
        ma_data_source_set_next(&decoders[iDecoder], &decoders[iDecoder+1]);
    }

    /*
    For the last data source we'll loop back to the start, but for demonstration purposes we'll use a
    callback to determine the next data source in the chain.
    */
    //ma_data_source_set_next_callback(&decoders[g_decoderCount-1], next_callback_tail);


    /*
    The data source chain has been established so now we can get the device up and running so we
    can listen to it.
    */
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = SAMPLE_FORMAT;
    deviceConfig.playback.channels = CHANNEL_COUNT;
    deviceConfig.sampleRate        = SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = NULL;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        goto done_decoders;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        goto done;
    }

    printf("Press Enter to add");
    getchar();

    result = ma_decoder_init_file(next.c_str(), &decoderConfig, &decoders[2]);
    ma_data_source_set_next(&decoders[1], &decoders[2]);
    printf("Press Enter to quit...");
    getchar();

done:
    ma_device_uninit(&device);

done_decoders:
    for (iDecoder = 0; iDecoder < g_decoderCount; ++iDecoder) {
        ma_decoder_uninit(&decoders[iDecoder]);
    }
    free(decoders);

    return 0;
}

int main() {
    std::vector<std::filesystem::path> paths;
    paths.push_back("../tracks/test_sound_1.mp3");
    paths.push_back("../tracks/test_sound_2.mp3");
    
    play(paths, "../tracks/eftos.mp3");
}