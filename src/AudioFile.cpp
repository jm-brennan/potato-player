#include "AudioFile.h"
#include <tag.h>
#include <tpropertymap.h>
#include <fileref.h>


void init(AudioFile& audioFile, std::string filename)
{
    audioFile.filename = filename;
    TagLib::FileRef f(filename.c_str());

    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        init(audioFile.displayTitle, tag->title().to8Bit());
        init(audioFile.displayArtistName, tag->artist().to8Bit());
        init(audioFile.displayAlbumName, tag->album().to8Bit());
    }
    else
    {
        std::cerr << "Could not find tags for file " << filename << "\n";
    }
}

void free_gl(AudioFile& audioFile) {
    free_gl(audioFile.displayArt);
    free_gl(audioFile.displayTitle);
    free_gl(audioFile.displayAlbumName);
    free_gl(audioFile.displayArtistName);
}

void generate_display_objects(AudioFile& audioFile, const FontData& largeFont, const FontData& smallFont) {
        
    audioFile.displayArt.model.pos = vec2(40.0, 40.0);
    audioFile.displayArt.size = vec2(316.0);

    generate_image_buffers(audioFile.displayArt);
    set_image_texture_from_audio_file(audioFile.displayArt, audioFile.filename);


    layout_text(audioFile.displayTitle, largeFont);
    float xpos = (800.0 / 2.0) - (audioFile.displayTitle.textStrip.width / 2.0);
    audioFile.displayTitle.model.pos = vec2(xpos, 396.0f);
    generate_text_strip_buffers(audioFile.displayTitle.textStrip);


    layout_text(audioFile.displayAlbumName, smallFont);
    audioFile.displayAlbumName.model.pos = vec2(396.0f, 200.0f);
    generate_text_strip_buffers(audioFile.displayAlbumName.textStrip);


    layout_text(audioFile.displayArtistName, smallFont);
    audioFile.displayArtistName.model.pos = vec2(396.0f, 120.0f);
    generate_text_strip_buffers(audioFile.displayArtistName.textStrip);

}

void render_audio_file_display(const AudioFile& audioFile, const FontData& largeFont, const FontData& smallFont, const Camera& camera) {
    render_image(audioFile.displayArt, camera);

    render_text(audioFile.displayTitle, largeFont, camera);
    render_text(audioFile.displayAlbumName, smallFont, camera);
    render_text(audioFile.displayArtistName, smallFont, camera);
}