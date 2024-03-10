#pragma once

#include "Text.h"
#include "definitions.h"
#include "Image.h"
#include "ColorQuad.h"

struct AudioFileDisplay {
    std::string filename;
    Text displayTitle;
    Image displayArt;
    Text displayAlbumName;
    Text displayArtistName;
    ColorQuad progressBar;
    ColorQuad progressIndicator;
    Text displayLength;
    Text displayProgress;
    float sampleRateInverse = 0.0f;
    uint lengthS = 0; // length in seconds
    uint displayTimeS = 0;
};

void init(AudioFileDisplay& audioFile, std::string filename);
void free_gl(AudioFileDisplay& audioFile);
void generate_display_objects(AudioFileDisplay& audioFile, const Fonts& fonts);
void update_playback_progress(AudioFileDisplay& audioFile, uint currentFrame, uint totalFrames, const Fonts& fonts);
void render_audio_file_display(const AudioFileDisplay& audioFile, const Fonts& fonts, bool isPlaying, const Camera& camera);