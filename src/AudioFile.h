#pragma once

#include "Text.h"
#include "definitions.h"
#include "Image.h"

struct AudioFile {
    std::string filename;
    Text displayTitle;
    Image displayArt;
    Text displayAlbumName;
    Text displayArtistName;
};

void init(AudioFile& audioFile, std::string filename);
void free_gl(AudioFile& audioFile);
void generate_display_objects(AudioFile& audioFile, const FontData& largeFont, const FontData& smallFont);
void render_audio_file_display(const AudioFile& audioFile, const FontData& largeFont, const FontData& smallFont, bool isPlaying, const Camera& camera);