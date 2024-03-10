#pragma once

#include "Text.h"
#include "definitions.h"
#include "Image.h"

struct AudioFileDisplay {
    std::string filename;
    Text displayTitle;
    Image displayArt;
    Text displayAlbumName;
    Text displayArtistName;
};

void init(AudioFileDisplay& audioFile, std::string filename);
void free_gl(AudioFileDisplay& audioFile);
void generate_display_objects(AudioFileDisplay& audioFile, const FontData& largeFont, const FontData& smallFont);
void render_audio_file_display(const AudioFileDisplay& audioFile, const FontData& largeFont, const FontData& smallFont, bool isPlaying, const Camera& camera);