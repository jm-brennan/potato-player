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
#include <map>
#include <array>
#include <wchar.h>

struct TextGlyph {
    vec2 texCoord; // 0 to 1 start of the glyph texture in the atlas
    vec2 size;
    vec2 advance;
    vec2 bearing; // offset from baseline to left/top of glyph
};

struct FontData {
    std::string fontFile;
    uint fontSizePx = 24;
    uint atlasTextureID = 0;
    vec2 atlasSize;
    std::vector<TextGlyph> ascii_glyphs;
    std::map<wchar_t, TextGlyph> non_ascii_glyphs;
};

enum FontIndex {
    MONO = 0,
    SMALL_ITALIC,
    MEDIUM,
    LARGE,
    JAPANESE,

    NUM_FONTS
};

using FontList = std::array<FontData, static_cast<size_t>(NUM_FONTS)>;

struct TextStrip {
    std::vector<TexturePoint> points;
    float width = 0.0;
    uint vertexBufferID = 0;
};

struct Text {
    std::wstring str;
    TextStrip textStrip;
    Model model;
    FontIndex fontIndex;
};

void text_init(Text& text, const std::wstring& str);
void free_gl(Text& text);
void free_gl(FontData& font);
void create_font(FontData& font, std::string fontName, uint size, std::vector<wchar_t> unicodeToCreate);
const TextGlyph* get_non_ascii_glyph(const FontData& font, const wchar_t c);
void layout_text(Text& text, FontList& fonts, FontIndex desiredFont);
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