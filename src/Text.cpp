#include "Text.h"

#include "gladInclude.h"
#include "ShaderManager.h"
#include "definitions.h"
#include "paths.h"

#include <glm/gtx/rotate_vector.hpp>

using namespace glm;

FT_Library freetype;

void init_freetype() {
    if (FT_Init_FreeType(&freetype)) {
        printf("ERROR::FreeType init\n");
        return;
    }
}

void shutdown_freetype() {
    FT_Done_FreeType(freetype);
}

void create_font(FontData& font, 
                 std::string fontName,
                 uint size,
                 std::vector<wchar_t> unicodeToCreate) {

    font.fontFile = fontName;
    font.fontSizePx = size;
    font.ascii_glyphs.resize(128);

    std::string filepath = std::string(FONT_DIR) + font.fontFile;
    if (FT_New_Face(freetype, filepath.c_str(), 0, &font.face)) {
        printf("ERROR::FreeType load font\n");
        return;
    }

    if (FT_Set_Pixel_Sizes(font.face, 0, size)) {
        printf("ERROR::FreeType pixel size\n");
    }

    // calculate width and height of single row atlas, store width for use during rendering, but keep
    // uints around for gl texture setup calls
    unsigned int atlasWidth  = 0;
    unsigned int atlasHeight = 0;
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(font.face, c, FT_LOAD_RENDER)) {
            printf("ERROR::FreeType load char [%c]\n", c);
            continue;
        }

        atlasWidth += font.face->glyph->bitmap.width + 1;
        atlasHeight = (font.face->glyph->bitmap.rows > atlasHeight) ? font.face->glyph->bitmap.rows : atlasHeight;
    }

    for (const wchar_t& c : unicodeToCreate) {
        if (FT_Get_Char_Index(font.face, c) == 0) {
            std::cout << "could not find character " << c << "in font";
        }

        std::cout << "getting face info for wchar: " << c << "\n";
        if (FT_Load_Char(font.face, c, FT_LOAD_RENDER)) {
            printf("ERROR::FreeType load char [%c]\n", c);
            continue;
        }

        std::cout << "got info\n";
        std::cout << "face ptr " << font.face << "\n";
        std::cout << "with glyph " << font.face->glyph << "\n";

        atlasWidth += font.face->glyph->bitmap.width + 1;
        atlasHeight = (font.face->glyph->bitmap.rows > atlasHeight) ? font.face->glyph->bitmap.rows : atlasHeight;
    }

    font.atlasSize = vec2(atlasWidth, atlasHeight);

    GLEC(glActiveTexture(GL_TEXTURE0));
    GLEC(glGenTextures(1, &font.atlasTextureID));
    std::cout << "Font texture buffer id " << font.atlasTextureID << " for " << font.fontFile << "\n";
    GLEC(glBindTexture(GL_TEXTURE_2D, font.atlasTextureID));

    // clamp to edge to prevent artifacts during scale
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    // linear filtering is good for text aparently
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    std::cout << "atlas size " << vec_string(font.atlasSize) << "\n";

    // TODO learn if this is really needed
    GLEC(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); // disable byte-alignment restriction since only using 1 byte to store bitmap
   
    GLEC(glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_ALPHA, 
        atlasWidth, 
        atlasHeight, 
        0, 
        GL_ALPHA, 
        GL_UNSIGNED_BYTE, 
        nullptr
    ));


    uint glyphStart = 0;
    for (uint c = 0; c < 128; c++) {
        if (FT_Load_Char(font.face, c, FT_LOAD_RENDER)) continue;
        GLEC(glTexSubImage2D(
            GL_TEXTURE_2D,
            0, 
            glyphStart,
            0, 
            font.face->glyph->bitmap.width,
            font.face->glyph->bitmap.rows,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            font.face->glyph->bitmap.buffer
        ));

        TextGlyph g = {
            vec2((float)glyphStart / font.atlasSize.x, 0.0f),
            vec2(font.face->glyph->bitmap.width, font.face->glyph->bitmap.rows),
            vec2(font.face->glyph->advance.x >> 6, 0.0f),
            vec2(font.face->glyph->bitmap_left, font.face->glyph->bitmap_top)
        };
        font.ascii_glyphs[c] = g;

        glyphStart += font.face->glyph->bitmap.width + 1;
    }

    for (const wchar_t& c : unicodeToCreate) {
        std::cout << "getting face info for wchar: " << c << "\n";
        if (FT_Load_Char(font.face, c, FT_LOAD_RENDER)) continue;
        GLEC(glTexSubImage2D(
            GL_TEXTURE_2D,
            0, 
            glyphStart,
            0, 
            font.face->glyph->bitmap.width,
            font.face->glyph->bitmap.rows,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            font.face->glyph->bitmap.buffer
        ));

        TextGlyph g = {
            vec2((float)glyphStart / font.atlasSize.x, 0.0f),
            vec2(font.face->glyph->bitmap.width, font.face->glyph->bitmap.rows),
            vec2(font.face->glyph->advance.x >> 6, 0.0f),
            vec2(font.face->glyph->bitmap_left, font.face->glyph->bitmap_top)
        };
        font.non_ascii_glyphs[c] = g;

        glyphStart += font.face->glyph->bitmap.width + 1;
    }

    return;
}

void free_gl(FontData& font) {
    GLEC(glDeleteTextures(1, &font.atlasTextureID));
}

void text_init(Text& text, const std::wstring& str, uint fontSizePx) {
    text.str = str;
    text.fontSizePx = fontSizePx;
}

void free_gl(Text& text) {
    GLEC(glDeleteBuffers(1, &text.textStrip.vertexBufferID));
}

FontIndex find_font_that_supports_glyphs(std::wstring& str, uint desiredSize, FontList& fonts, std::vector<wchar_t>& unseenGlyphs) {
    uint bestScore = 0;
    FontIndex bestIndex = (FontIndex)0;
    std::string bestFontName = "";

    for (uint i = 0; i < fonts.size(); ++i) {
        if (fonts[i].empty()) {
            continue; // this font not in use, in theory we could initialize it here though and proceed
        }

        FontData& font = fonts[i].begin()->second;
        uint fontScore = 0;
        for (const wchar_t& c : str) {
            if (FT_Load_Char(font.face, c, FT_LOAD_RENDER)) {
                printf("ERROR::FreeType load char [%c]\n", c);
                continue;
            }
            if (FT_Get_Char_Index(font.face, c) != 0) {
                ++fontScore;
            }
        }

        if (fontScore == str.size()) {
            bestScore = fontScore;
            bestIndex = (FontIndex)i;
            bestFontName = font.fontFile;
            break;
        }
        else if (fontScore > bestScore) {
            bestScore = fontScore;
            bestIndex = (FontIndex)i;
            bestFontName = font.fontFile;
        }
    }

    if (fonts[bestIndex].find(desiredSize) == fonts[bestIndex].end()) {
        create_font(fonts[bestIndex][desiredSize], bestFontName, desiredSize, {});
    }
    // TODO iterators to avoid multiple lookups
    FontData& bestFont = fonts[bestIndex][desiredSize];
    
    for (const wchar_t& c : str) {
        if (c > 127 && bestFont.non_ascii_glyphs.find(c) == bestFont.non_ascii_glyphs.end()) {
            unseenGlyphs.push_back(c);
        }
    }

    return bestIndex;
}

void layout_text(Text& text, FontList& fonts, FontIndex desiredFontIndex, uint fontSizePx) {
    std::wcout << "trying to layout " << text.str << " with font " << desiredFontIndex << " at size " << fontSizePx << "\n";
    //if (fonts[determinedFont].find(fontSizePx) == fonts[determinedFont].end()) {
    //    
    //}

    vec2 pos = vec2(0.0f, 0.0f);

    text.textStrip.points.resize(text.str.size() * 6); // 6 points per quad since we dont use indexing
    text.textStrip.width = 0;

    std::vector<wchar_t> unseenGlyphs;
    bool anyGlyphsUnsupportedByFont = false;
    FontData& desiredFont = fonts[desiredFontIndex][text.fontSizePx];
    for (const wchar_t& c : text.str) {
        if (c < 128) {
            continue;
        }

        if (desiredFont.non_ascii_glyphs.find(c) == desiredFont.non_ascii_glyphs.end()) {
            if (FT_Load_Char(desiredFont.face, c, FT_LOAD_RENDER)) {
                printf("ERROR::FreeType load char [%c]\n", c);
                continue;
            }
            if (FT_Get_Char_Index(desiredFont.face, c) == 0) {
                std::cout << "could not find character " << c << "in desired font\n";
                anyGlyphsUnsupportedByFont = true;
            }
            else {
                std::cout << "found char that is not yet glyphed but is supported by font: " << c << "\n";
                unseenGlyphs.push_back(c);
            }
        }
    }

    FontIndex determinedFont = desiredFontIndex;

    if (anyGlyphsUnsupportedByFont) {
        unseenGlyphs.clear();
        determinedFont = find_font_that_supports_glyphs(text.str, text.fontSizePx, fonts, unseenGlyphs);
    }
    
    FontData& font = fonts[determinedFont][fontSizePx];
    text.fontIndex = determinedFont;
    
    if (!unseenGlyphs.empty()) {
        std::cout << "recreating font\n";
        free_gl(font);
        create_font(font, font.fontFile, font.fontSizePx, unseenGlyphs);
    }
    
    uint i = 0; // textStrip points array index
    for (const wchar_t& c : text.str) {
        const TextGlyph* glyph = nullptr;
        if (c < 128) {
            glyph = &font.ascii_glyphs[c];
        }
        else {
            glyph = &font.non_ascii_glyphs[c];
        }

        if (!glyph) {
            std::cout << "Bad glyph data for " << c << "\n";
            continue;
        }

        std::cout << "layout for character " << c << " with font " << desiredFontIndex << " and size " << fontSizePx << "\n";
        float x = pos.x + glyph->bearing.x;
        float y = pos.y - (glyph->size.y - glyph->bearing.y);//+ (font.atlasSize.y - glyph->size.y) + (glyph->size.y - glyph->bearing.y);
        float w = glyph->size.x;
        float h = glyph->size.y;

        text.textStrip.points[i++] = (TexturePoint){vec2(x, y),       vec2(glyph->texCoord.x, glyph->texCoord.y + (glyph->size.y / font.atlasSize.y))};
        text.textStrip.points[i++] = (TexturePoint){vec2(x, y+h),     vec2(glyph->texCoord.x, glyph->texCoord.y)};
        text.textStrip.points[i++] = (TexturePoint){vec2(x + w, y+h), vec2(glyph->texCoord.x + (glyph->size.x / font.atlasSize.x), glyph->texCoord.y)};
        text.textStrip.points[i++] = (TexturePoint){vec2(x, y),       vec2(glyph->texCoord.x, glyph->texCoord.y + (glyph->size.y / font.atlasSize.y))};
        text.textStrip.points[i++] = (TexturePoint){vec2(x + w, y+h), vec2(glyph->texCoord.x + (glyph->size.x / font.atlasSize.x), glyph->texCoord.y)};
        text.textStrip.points[i++] = (TexturePoint){vec2(x + w, y),   vec2(glyph->texCoord.x + (glyph->size.x / font.atlasSize.x), glyph->texCoord.y + (glyph->size.y / font.atlasSize.y))};
        pos += glyph->advance;

        text.textStrip.width += glyph->advance.x;
    }


    /* for (TexturePoint tp : points) {
        std::cout << vec_string(tp.vertex) << " : " << vec_string(tp.texture) << "\n";
    } */

}

void generate_text_strip_buffers(TextStrip& textStrip) {
    ShaderManager::use(TEXT);
    GLEC(glGenBuffers(1, &textStrip.vertexBufferID));

    std::cout << "text strip size " << sizeof(TexturePoint)
             << "\nnum points " << textStrip.points.size()
             << "\n";

    GLEC(glBindBuffer(GL_ARRAY_BUFFER, textStrip.vertexBufferID));
    std::cout << "text strip vertex buffer id " << textStrip.vertexBufferID << "\n";
    GLEC(glBufferData(GL_ARRAY_BUFFER, textStrip.points.size() * sizeof(TexturePoint), textStrip.points.data(), GL_STATIC_DRAW));
}

void render_text(const Text& text, const FontData& font, const Camera& camera) {
    ShaderManager::use(TEXT);
    GLEC(glActiveTexture(GL_TEXTURE0));
    GLEC(glBindTexture(GL_TEXTURE_2D, font.atlasTextureID));
    GLEC(glBindBuffer(GL_ARRAY_BUFFER, text.textStrip.vertexBufferID));

    GLEC(glEnableVertexAttribArray(0));
    GLEC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)0));
    GLEC(glBindAttribLocation(ShaderManager::program(TEXT), 0, "a_position"));

    GLEC(glEnableVertexAttribArray(1));
    GLEC(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)(2*sizeof(GL_FLOAT))));
    GLEC(glBindAttribLocation(ShaderManager::program(TEXT), 1, "a_texCoord"));
    //GLEC(glLinkProgram(ShaderManager::program(TEXT)));

    GLEC(glBindBuffer(GL_ARRAY_BUFFER, 0));

    // TODO probably we dont need to recalculate this every frame but its easier for now at least
    glm::mat4 model(1.0f);
    
    model = translate(model, vec3(text.model.pos.x, text.model.pos.y, 0.0f));

    glm::mat4 mvp = camera.viewProjection * model;

    //std::cout << "vp:\n" << mat_string(camera.viewProjection) << "\n";

    GLEC(glUniformMatrix4fv(glGetUniformLocation(ShaderManager::program(TEXT), "m_mvp"), 1, false, value_ptr(mvp)));

    GLEC(glDrawArrays(GL_TRIANGLES, 0, text.textStrip.points.size()));

}