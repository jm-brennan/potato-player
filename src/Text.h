#include <ft2build.h>
#include FT_FREETYPE_H 

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include "definitions.h"
#include "gladInclude.h"
#include "Texture.h"

struct TextStrip {
    std::vector<TexturePoint> points;
    uint vertexBufferID = 0;
    uint indexBufferID = 0;
};

struct TextGlyph {
    vec2 texCoord; // 0 to 1 start of the glyph texture in the atlas
    vec2 size;
    vec2 advance;
    vec2 bearing; // offset from baseline to left/top of glyph
};

struct FontData {
    uint atlasTextureID = 0;
    vec2 atlasSize;
    std::vector<TextGlyph> glyphs;
};

FontData create_font(std::string font);
std::vector<TexturePoint> layout_text(const std::string& layoutString, FontData& font);
void generate_text_strip_buffers(TextStrip& textStrip);
void render_text(TextStrip& textStrip, vec2 location, FontData& font);


inline std::string text_glyph_string(const TextGlyph& glyph) {
    std::string result;
    result += "Texture coordinate: " + vec_string(glyph.texCoord) 
            + " size: " + vec_string(glyph.size)
            + " advance: " + vec_string(glyph.advance)
            + " bearing: " + vec_string(glyph.bearing);
    return result;
}