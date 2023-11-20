
#include <filesystem>
#include <stdio.h>
#include <vector>
#include <atomic>
#include "miniaudio.h"


extern ma_decoder_config decoderConfig;
extern std::vector<std::filesystem::path> paths;
extern std::atomic<uint> pathsIndex;
extern ma_decoder* activeDecoder;
extern ma_decoder* nextDecoder;
extern std::atomic<float> currentTrackProgress;

//int play(std::vector<std::filesystem::path> paths);

void uninitialize_device(ma_device& device);

void uninitialize_decoders();

void init_decoder_config();

int start_playlist_playback(ma_device_config& deviceConfig,
                            ma_device& device, 
                            std::vector<std::filesystem::path> audioPaths);