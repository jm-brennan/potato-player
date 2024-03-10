#include <ft2build.h>
#include FT_FREETYPE_H 

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include "definitions.h"
#include "gladInclude.h"
#include "Texture.h"
#include "Model.h"
#include "Camera.h"

struct TextStrip {
    std::vector<TexturePoint> points;
    float width = 0.0;
    uint vertexBufferID = 0;
};

struct TextGlyph {
    vec2 texCoord; // 0 to 1 start of the glyph texture in the atlas
    vec2 size;
    vec2 advance;
    vec2 bearing; // offset from baseline to left/top of glyph
};

struct Text {
    std::string str;
    TextStrip textStrip;
    Model model;
};

struct FontData {
    uint atlasTextureID = 0;
    vec2 atlasSize;
    std::vector<TextGlyph> glyphs;
};

struct Fonts {
    FontData large;
    FontData medium;
    FontData smallItalic;
    FontData mono;
};

void init(Text& text, const std::string& str);
void free_gl(Text& text);
void free_gl(FontData& font);
FontData create_font(std::string font, uint size);
void layout_text(Text& text, const FontData& font);
void generate_text_strip_buffers(TextStrip& textStrip);
void render_text(const Text& text, const FontData& font, const Camera& camera);


inline std::string text_glyph_string(const TextGlyph& glyph) {
    std::string result;
    result += "Texture coordinate: " + vec_string(glyph.texCoord) 
            + " size: " + vec_string(glyph.size)
            + " advance: " + vec_string(glyph.advance)
            + " bearing: " + vec_string(glyph.bearing);
    return result;
}