#include "Text.h"

#include "gladInclude.h"
#include "ShaderManager.h"
#include "definitions.h"
#include "paths.h"

#include <glm/gtx/rotate_vector.hpp>

using namespace glm;

FontData create_font(std::string font) {
    FontData result;
    result.glyphs.resize(128);

    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        printf("ERROR::FreeType init\n");
        return result;
    }

    FT_Face face;
    std::string filepath = std::string(FONT_DIR) + font;
    if (FT_New_Face(library, filepath.c_str(), 0, &face)) {
        printf("ERROR::FreeType load font\n");
        return result;
    }

    if (FT_Set_Pixel_Sizes(face, 0, 32)) {
        printf("ERROR::FreeType pixel size\n");
    }

    // calculate width and height of single row atlas, store width for use during rendering, but keep
    // uints around for gl texture setup calls
    unsigned int atlasWidth  = 0;
    unsigned int atlasHeight = 0;
    for (unsigned char c = 0; c < 128; c++) {
         if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            printf("ERROR::FreeType load char [%c]\n", c);
            continue;
        }

        atlasWidth += face->glyph->bitmap.width + 1;
        atlasHeight = (face->glyph->bitmap.rows > atlasHeight) ? face->glyph->bitmap.rows : atlasHeight;
    }

    result.atlasSize = vec2(atlasWidth, atlasHeight);

    GLEC(glActiveTexture(GL_TEXTURE0));
    GLEC(glGenTextures(1, &result.atlasTextureID));
    std::cout << "Font texture buffer id " << result.atlasTextureID << " for " << font << "\n";
    GLEC(glBindTexture(GL_TEXTURE_2D, result.atlasTextureID));

    // clamp to edge to prevent artifacts during scale
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    // linear filtering is good for text aparently
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    std::cout << "atlas size " << vec_string(result.atlasSize) << "\n";

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
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        GLEC(glTexSubImage2D(
            GL_TEXTURE_2D,
            0, 
            glyphStart,
            0, 
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        ));

        TextGlyph g = {
            vec2((float)glyphStart / result.atlasSize.x, 0.0f),
            vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            vec2(face->glyph->advance.x >> 6, 0.0f),
            vec2(face->glyph->bitmap_left, face->glyph->bitmap_top)
        };
        result.glyphs[c] = g;

        glyphStart += face->glyph->bitmap.width + 1;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return result;
}

std::vector<TexturePoint> layout_text(const std::string& layoutString, FontData& font) {

    vec2 pos; // start at 0,0

    std::vector<TexturePoint> points;
    points.resize(layoutString.size() * 6); // 6 points per quad since we dont use indexing

    uint i = 0; // to index into points array
    for (const char& c : layoutString) {
        TextGlyph& glyph = font.glyphs[c];

        float x = pos.x + glyph.bearing.x;
        float y = pos.y + (font.atlasSize.y - glyph.size.y) + (glyph.size.y - glyph.bearing.y);
        float w = glyph.size.x;
        float h = glyph.size.y;

        double scaleFactor = 400.0;
        x /= scaleFactor;
        y /= scaleFactor;
        w /= scaleFactor;
        h /= scaleFactor;

        points[i++] = (TexturePoint){vec2(x, y),        vec2(glyph.texCoord.x, glyph.texCoord.y)};
        points[i++] = (TexturePoint){vec2(x, y+h),      vec2(glyph.texCoord.x, glyph.texCoord.y + (glyph.size.y / font.atlasSize.y))};
        points[i++] = (TexturePoint){vec2(x + w, y+h),  vec2(glyph.texCoord.x + (glyph.size.x / font.atlasSize.x), glyph.texCoord.y + (glyph.size.y / font.atlasSize.y))};
        points[i++] = (TexturePoint){vec2(x, y),        vec2(glyph.texCoord.x, glyph.texCoord.y)};
        points[i++] = (TexturePoint){vec2(x + w, y+h),  vec2(glyph.texCoord.x + (glyph.size.x / font.atlasSize.x), glyph.texCoord.y + (glyph.size.y / font.atlasSize.y))};
        points[i++] = (TexturePoint){vec2(x + w, y),    vec2(glyph.texCoord.x + (glyph.size.x / font.atlasSize.x), glyph.texCoord.y)};
        pos += glyph.advance;
    }

    /* for (TexturePoint tp : points) {
        std::cout << vec_string(tp.vertex) << " : " << vec_string(tp.texture) << "\n";
    } */

    return points;
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

    for (TexturePoint tp : textStrip.points) {
        std::cout << vec_string(tp.vertex) << " : " << vec_string(tp.texture) << "\n";
    }

}

void render_text(Text& text, FontData& font) {
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
    glm::mat4 mvp(1.0f);
    mvp = translate(mvp, vec3(text.model.pos, 0.0f));

    GLEC(glUniformMatrix4fv(glGetUniformLocation(ShaderManager::program(TEXT), "m_mvp"), 1, false, value_ptr(mvp)));

    GLEC(glDrawArrays(GL_TRIANGLES, 0, text.textStrip.points.size()));

}