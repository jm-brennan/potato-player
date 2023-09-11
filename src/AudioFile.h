#pragma once

#include "Text.h"
#include "Image.h"

struct AudioFile {
    std::string filename;
    Text displayTitle;
    Image displayArt;
    Text displayAlbumName;
    Text displayArtistName;
};

void init(AudioFile& audioFile, std::string filename);
void generate_display_objects(AudioFile& audioFile, const FontData& largeFont, const FontData& smallFont);
void render_audio_file_display(const AudioFile& audioFile, const FontData& largeFont, const FontData& smallFont, const Camera& camera);