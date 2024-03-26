#include "AudioFileDisplay.h"
#include <tag.h>
#include <tpropertymap.h>
#include <fileref.h>

std::string seconds_to_display_time(uint seconds);

void audio_file_init(AudioFileDisplay& audioFile, std::string filename)
{
    audioFile.filename = filename;
    TagLib::FileRef f(filename.c_str());

    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        text_init(audioFile.displayTitle, tag->title().toWString());
        text_init(audioFile.displayArtistName, tag->artist().toWString());
        text_init(audioFile.displayAlbumName, tag->album().toWString());
        init(audioFile.progressBar, vec2(396.0f, 50.0f), vec2(364.0f, 2.0f), vec4(0.9f, 0.6f, 0.9f, 1.0f));
        init(audioFile.progressIndicator, vec2(396.0f, 40.0f), vec2(4.0f, 20.0f), vec4(0.9f, 0.6f, 0.9f, 1.0f));

        if (f.audioProperties()) {
            audioFile.sampleRateInverse = 1.0f / (float)f.audioProperties()->sampleRate();
            //std::cout << "file has sample rate of " << 1.0f / audioFile.sampleRateInverse << "\n";
            audioFile.lengthS = f.audioProperties()->lengthInSeconds();
        }

        text_init(audioFile.displayLength, wsconverter.from_bytes("/" + seconds_to_display_time(audioFile.lengthS)));
        text_init(audioFile.displayProgress, wsconverter.from_bytes(seconds_to_display_time(audioFile.displayTimeS)));
    }
    else
    {
        std::cerr << "Could not find tags for file " << filename << "\n";
    }
}

void free_gl(AudioFileDisplay& audioFile) {
    free_gl(audioFile.displayArt);
    free_gl(audioFile.displayTitle);
    free_gl(audioFile.displayAlbumName);
    free_gl(audioFile.displayArtistName);
}

void generate_display_objects(AudioFileDisplay& audioFile, FontList& fonts) {
        
    audioFile.displayArt.model.pos = vec2(40.0, 40.0);
    audioFile.displayArt.size = vec2(316.0);

    generate_image_buffers(audioFile.displayArt);
    set_image_texture_from_audio_file(audioFile.displayArt, audioFile.filename);


    layout_text(audioFile.displayTitle, fonts, FontIndex::LARGE);
    float xpos = (800.0 / 2.0) - (audioFile.displayTitle.textStrip.width / 2.0);
    audioFile.displayTitle.model.pos = vec2(xpos, 396.0f);
    generate_text_strip_buffers(audioFile.displayTitle.textStrip);

    layout_text(audioFile.displayArtistName, fonts, FontIndex::MEDIUM);
    audioFile.displayArtistName.model.pos = vec2(396.0f, 240.0f);
    generate_text_strip_buffers(audioFile.displayArtistName.textStrip);

    layout_text(audioFile.displayAlbumName, fonts, FontIndex::SMALL_ITALIC);
    audioFile.displayAlbumName.model.pos = vec2(396.0f, 200.0f);
    generate_text_strip_buffers(audioFile.displayAlbumName.textStrip);

    layout_text(audioFile.displayLength, fonts, FontIndex::MONO);
    audioFile.displayLength.model.pos = vec2(760.0f - audioFile.displayLength.textStrip.width, 80.0f);
    generate_text_strip_buffers(audioFile.displayLength.textStrip);
    
    layout_text(audioFile.displayProgress, fonts, FontIndex::MONO);
    audioFile.displayProgress.model.pos = vec2(audioFile.displayLength.model.pos.x - audioFile.displayProgress.textStrip.width, 80.0f);
    generate_text_strip_buffers(audioFile.displayProgress.textStrip);
}

void update_playback_progress(AudioFileDisplay& audioFile, uint currentFrame, uint totalFrames, FontList& fonts) {
    float barSize = audioFile.progressBar.size.x;
    float progressPercent = (float)currentFrame / (float)totalFrames;
    
    audioFile.progressIndicator.pos.x = audioFile.progressBar.pos.x + (barSize * progressPercent);

    uint displayTimeS = std::floor(progressPercent * audioFile.lengthS);
    if (displayTimeS != audioFile.displayTimeS)
    {
        audioFile.displayTimeS = displayTimeS;
        free_gl(audioFile.displayProgress);
        text_init(audioFile.displayProgress, wsconverter.from_bytes(seconds_to_display_time(displayTimeS)));
        layout_text(audioFile.displayProgress, fonts, FontIndex::MONO);
        audioFile.displayProgress.model.pos = vec2(audioFile.displayLength.model.pos.x - audioFile.displayProgress.textStrip.width, 80.0f);
        generate_text_strip_buffers(audioFile.displayProgress.textStrip);
    }
}

std::string seconds_to_display_time(uint seconds) {

    uint hours = seconds / 3600;
    seconds -= hours * 3600;
    uint minutes = seconds / 60;
    seconds -= minutes * 60;

    std::string result = "";
    if (hours > 0) {
        result += std::to_string(hours) + ":";
    }

    // always display minutes even if 0
    if (hours > 0 && minutes < 10) {
        result += "0";
    }
    result += std::to_string(minutes) + ":";

    if (seconds < 10) {
        result += "0";
    }

    result += std::to_string(seconds);

    return result;
}

void render_audio_file_display(const AudioFileDisplay& audioFile, FontList& fonts, bool isPlaying, const Camera& camera) {
    render_image(audioFile.displayArt, camera);

    render_text(audioFile.displayTitle, fonts[audioFile.displayTitle.fontIndex], camera);
    render_text(audioFile.displayArtistName, fonts[audioFile.displayArtistName.fontIndex], camera);
    render_text(audioFile.displayAlbumName, fonts[audioFile.displayAlbumName.fontIndex], camera);
    render_text(audioFile.displayLength, fonts[audioFile.displayLength.fontIndex], camera);
    render_text(audioFile.displayProgress, fonts[audioFile.displayProgress.fontIndex], camera);

    render_color_quad(audioFile.progressBar, camera);
    render_color_quad(audioFile.progressIndicator, camera);
}