#include "Text.h"

#include "gladInclude.h"
#include "ShaderManager.h"
#include "definitions.h"
#include "paths.h"

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
    GLEC(glBindTexture(GL_TEXTURE_2D, result.atlasTextureID));

    // clamp to edge to prevent artifacts during scale
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    // linear filtering is good for text
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

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


    unsigned int glyphStart = 0;
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
    points.resize(layoutString.size() * 4); // 4 points per quad since we use indexing

    uint i = 0; // to index into points array
    for (const char& c : layoutString) {
        TextGlyph& glyph = font.glyphs[c];

        float x = pos.x + glyph.bearing.x;
        float y = pos.y + (font.atlasSize.y - glyph.size.y) + (glyph.size.y - glyph.bearing.y);
        float w = glyph.size.x;
        float h = glyph.size.y;
        
        points[i++] = (TexturePoint){vec2(x, y),        vec2(glyph.texCoord.x, glyph.texCoord.y)};
        points[i++] = (TexturePoint){vec2(x, y+h),      vec2(glyph.texCoord.x, glyph.texCoord.y + (h / font.atlasSize.y))};
        points[i++] = (TexturePoint){vec2(x + w, y+h),  vec2(glyph.texCoord.x + (w / font.atlasSize.x), glyph.texCoord.y + (h / font.atlasSize.y))};
        points[i++] = (TexturePoint){vec2(x + w, y),    vec2(glyph.texCoord.x + (w / font.atlasSize.x), glyph.texCoord.y)};
        pos += glyph.advance;
    }

    return points;
}

void generate_text_strip_buffers(TextStrip& textStrip) {
    GLEC(glGenBuffers(1, &textStrip.vertexBufferID));
    GLEC(glGenBuffers(1, &textStrip.indexBufferID));

    GLEC(glBindBuffer(GL_ARRAY_BUFFER, textStrip.vertexBufferID));
    GLEC(glBufferData(GL_ARRAY_BUFFER, textStrip.points.size() * sizeof(TexturePoint), textStrip.points.data(), GL_STATIC_DRAW));


    std::vector<uint> indices;
    // divide by 4 to find number of quads, then multiply by 6 for 6 indices per quad
    indices.resize((textStrip.points.size() / 4) * 6);

    const uint INDICES_PER_ITERATION = 6;
    uint i = 0;
    while (i <= indices.size() - INDICES_PER_ITERATION) {
        // first triangle relative indices 0, 1, 3
        indices[i]   = i;
        indices[i+1] = i + 1;
        indices[i+2] = i + 3;

        // second triangle relative indices 1, 2, 3
        indices[i+3] = i + 1;
        indices[i+4] = i + 2;
        indices[i+5] = i + 3;

        i += INDICES_PER_ITERATION;
    }

    GLEC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textStrip.indexBufferID));
    GLEC(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW));

    GLEC(glEnableVertexAttribArray(0));
    GLEC(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)0));
    GLEC(glBindAttribLocation(ShaderManager::program(TEXT), 0, "a_position"));

    GLEC(glEnableVertexAttribArray(1));
    GLEC(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)(2*sizeof(GL_FLOAT))));
    GLEC(glBindAttribLocation(ShaderManager::program(TEXT), 1, "a_texCoord"));
    GLEC(glLinkProgram(ShaderManager::program(TEXT)));
}


void render_text(TextStrip& textStrip, vec2 location, FontData& font) {
    ShaderManager::use(TEXT);
    GLEC(glActiveTexture(GL_TEXTURE0));
    GLEC(glBindTexture(GL_TEXTURE_2D, font.atlasTextureID));

    // convert points to a buffer

    //GLEC(glBindTexture(GL_TEXTURE_2D, font.atlasTextureID));
	//GLEC(glBindVertexArray(textStrip.vertexBufferID));
	//GLEC(glBindBuffer(GL_ARRAY_BUFFER, textStrip.indexBufferID));

    glDrawElements(GL_TRIANGLES, (textStrip.points.size() / 4) * 6, GL_UNSIGNED_INT, 0);

	/* GLEC(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));

    GLEC(glEnableVertexAttribArray(0));
    GLEC(glBufferData(GL_ARRAY_BUFFER, coordsSize*4*4, (float*)coords, GL_DYNAMIC_DRAW));
    GLEC(glDrawArrays(GL_TRIANGLES, 0, coordsSize));
    
    GLEC(glBindVertexArray(0));
    GLEC(glBindTexture(GL_TEXTURE_2D, 0)); */
}