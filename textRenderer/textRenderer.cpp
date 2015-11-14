
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include <string>
#include <assert.h>
#include "textRenderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H

/// global freetype structs
static FT_Library   ft_library;
static FT_Face      ft_face;
static FT_GlyphSlot ft_glyph;

//global GL objects
static GLuint fontTexID;
static GLuint fontVAO;
static GLuint fontVBO;
static GLuint fontProgram;

//total atlas dimensions
static unsigned int ft_atlasWidth, ft_atlasHeight;

//packed character bitmap info
struct ft_character_info {
  float ax; // advance.x
  float ay; // advance.y

  float bw; // bitmap.width;
  float bh; // bitmap.rows;

  float bl; // bitmap_left;
  float bt; // bitmap_top;

  float tx; // x offset of glyph in texture coordinates
} ft_char_buffer[128];

/* ************ GL 3.3 shader code *************** */
static const std::string font_gl33_vertexShaderString(
"#version 330\n"

"in vec4 font_coord;\n"
"out vec2 font_texcoord;\n"

"void main(){\n"
"    gl_Position = vec4(font_coord.xy, 0, 1);\n"
"    font_texcoord = font_coord.zw;\n"
"}");

static const std::string font_gl33_fragmentShaderString(
"#version 330\n"

"in vec2 font_texcoord;\n"
"uniform sampler2D font_tex;\n"
"uniform vec4 font_color;\n"

"out vec4 font_outcolor;\n"

"void main(){\n"
"    font_outcolor = vec4(1, 1, 1, texture(font_tex, font_texcoord).r)*font_color;\n"
"}");

/* ************ GL 2.1 shader code *************** */

static const std::string font_gl21_vertexShaderString(
"#version 120\n"

"attribute vec4 font_coord;\n"
"varying vec2 font_texcoord;\n"

"void main(){\n"
"    gl_Position = vec4(font_coord.xy, 0, 1);\n"
"    font_texcoord = font_coord.zw;\n"
"}");

static const std::string font_gl21_fragmentShaderString(
"#version 120\n"

"varying vec2 font_texcoord;\n"
"uniform sampler2D font_tex;\n"
"uniform vec4 font_color;\n"

"void main(){\n"
"    gl_FragColor = vec4(1, 1, 1, texture2D(font_tex, font_texcoord).r)*font_color;\n"
"}");

/* ************* /shader code ************ */

void
textRenderInit(std::string font_filename, int fontSize)
{
    /// initialize the library
    if(FT_Init_FreeType(&ft_library)) {
        std::cout<< "Could not init ft"<<std::endl;
    }

    if(FT_New_Face(ft_library, font_filename.c_str(), 0, &ft_face)) {
        std::cout<<"ft2 face error"<<std::endl;
    }

    /// subject to change at runtime
    FT_Set_Pixel_Sizes(ft_face, 0, fontSize);

    /// generate a texture object and set its parameters
    glGenTextures(1, &fontTexID);
    glBindTexture(GL_TEXTURE_2D, fontTexID);
    glActiveTexture(GL_TEXTURE0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* ********* shader compilation, program linking etc ********* */
    GLuint font_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *font_shaderSource;

    /// Choose GLSL version depending on gl version
    if(g_glMajorVersion <= 3 && g_glMinorVersion < 3) {
        font_shaderSource = font_gl21_vertexShaderString.c_str();
    } else {
        font_shaderSource = font_gl33_vertexShaderString.c_str();
    }

    glShaderSource(font_vertexShader, 1, &font_shaderSource, 0);
    glCompileShader(font_vertexShader);
    GLint isCompiled = GL_FALSE;
    glGetShaderiv(font_vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(font_vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<char> errorLog(maxLength);
        glGetShaderInfoLog(font_vertexShader, maxLength, &maxLength, &errorLog[0]);
        for (int i=0; i<maxLength; ++i) {std::cout<<errorLog[i];}
        glDeleteShader(font_vertexShader);
    }

    GLuint font_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    /// Choose GLSL version depending on gl version
    if(g_glMajorVersion <= 3 && g_glMinorVersion < 3) {
        font_shaderSource = font_gl21_fragmentShaderString.c_str();
    } else {
        font_shaderSource = font_gl33_fragmentShaderString.c_str();
    }

    glShaderSource(font_fragmentShader, 1, &font_shaderSource, 0);
    glCompileShader(font_fragmentShader);
    isCompiled = GL_FALSE;
    glGetShaderiv(font_fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(font_fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<char> errorLog(maxLength);
        glGetShaderInfoLog(font_fragmentShader, maxLength, &maxLength, &errorLog[0]);
        for (int i=0; i<maxLength; ++i) {std::cout<<errorLog[i];}
        glDeleteShader(font_vertexShader);
        glDeleteShader(font_fragmentShader);
    }

    fontProgram = glCreateProgram();
    glAttachShader(fontProgram, font_vertexShader);
    glAttachShader(fontProgram, font_fragmentShader);
    glLinkProgram(fontProgram);

    GLint isLinked = 0;
    glGetProgramiv(fontProgram, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE){
        std::string msg("Program linking failure: ");
        GLint infoLogLength;
        glGetProgramiv(fontProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(fontProgram, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;
        std::cout<<msg<<std::endl;
        glDeleteProgram(fontProgram);
        glDeleteShader(font_vertexShader);
        glDeleteShader(font_fragmentShader);
    }

    glDetachShader(fontProgram, font_vertexShader);
    glDetachShader(fontProgram, font_fragmentShader);
    glDeleteShader(font_vertexShader);
    glDeleteShader(font_fragmentShader);
    /* ********* /shader compilation, program linking etc ********* */

    /// generate the text's VAO and VBO
    glGenVertexArrays(1, &fontVAO);
    glBindVertexArray(fontVAO);

    glGenBuffers(1, &fontVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fontVBO);

    /// calculate the total atlas' width and height
    for(unsigned int i = 32; i < 128; i++) {
        if(FT_Load_Char(ft_face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            assert(0);
            continue;
        }
        ft_glyph = ft_face->glyph;
        ft_atlasWidth += ft_glyph->bitmap.width+1;

        /// atlas' height is the maximum height of all the characters
        if (ft_glyph->bitmap.rows>ft_atlasHeight)
            ft_atlasHeight=ft_glyph->bitmap.rows;
    }

    /// set 1-byte pixel alignment (default is 4 - RGBA)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /// allocate an ft_atlasWidth*ft_atlasHeight empty texture
    /// use only the red channel
    /// make sure memory is initialized properly with 0s
    unsigned char pixels[ft_atlasWidth*ft_atlasHeight];
    memset(pixels, 0, sizeof(pixels));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ft_atlasWidth, ft_atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

    /// get all the glyph info (used later for the vertex data)
    /// and upload the bitmaps
    int x_offset = 0;
    for(unsigned int i = 32; i < 128; i++) {
        if(FT_Load_Char(ft_face, i, FT_LOAD_RENDER)){
            std::cout<<"ft2 loading char err"<<std::endl;
            continue;
        }

        ft_glyph = ft_face->glyph;

        glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, 0,
                        ft_glyph->bitmap.width, ft_glyph->bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE, ft_glyph->bitmap.buffer);

        //advance.x,y are in 1/64 pixels so divide by 64
        ft_char_buffer[i].ax = ft_glyph->advance.x >> 6;
        ft_char_buffer[i].ay = ft_glyph->advance.y >> 6;

        ft_char_buffer[i].bw = ft_glyph->bitmap.width;
        ft_char_buffer[i].bh = ft_glyph->bitmap.rows;

        ft_char_buffer[i].bl = ft_glyph->bitmap_left;
        ft_char_buffer[i].bt = ft_glyph->bitmap_top;

        ft_char_buffer[i].tx = x_offset / (float)ft_atlasWidth;

        x_offset += ft_glyph->bitmap.width + 1;
    }

    /// 4 floats per coordinate - x,y,u,v
    glEnableVertexAttribArray(glGetAttribLocation(fontProgram, "font_coord"));
    glVertexAttribPointer(glGetAttribLocation(fontProgram, "font_coord"),
                4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /// restore the pixel alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void
textRenderDrawText(std::string text, float x, float y, float sx, float sy, float textColor[4])
{
    GLint currentProgram = -1;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    assert(currentProgram >= 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(fontVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
    glUseProgram(fontProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexID);
    glUniform1i(glGetUniformLocation(fontProgram, "font_tex"), 0);
    glUniform4fv(glGetUniformLocation(fontProgram, "font_color"), 1, textColor);

    /// holds the x,y,u,v info for each point
    struct point {
        GLfloat x;
        GLfloat y;
        GLfloat s;
        GLfloat t;
    } coords[6 * text.size()];

    /// calculate the new vertex data for each character
    /// and update the buffer
    int n = 0;
    int idx;
    for(unsigned int i=0; i < text.size(); i++) {
        idx = int(text[i]);
        float x2 =  x + ft_char_buffer[idx].bl * sx;
        float y2 = -y - ft_char_buffer[idx].bt * sy;
        float w = ft_char_buffer[idx].bw * sx;
        float h = ft_char_buffer[idx].bh * sy;

        /// advance to the position of the next character
        x += ft_char_buffer[idx].ax * sx;
        y += ft_char_buffer[idx].ay * sy;

        /// 1 quad, 2 triangles, 6 points
        coords[n    ] = point{x2,     -y2    , ft_char_buffer[idx].tx,                                          0};
        coords[n + 2] = point{x2 + w, -y2    , ft_char_buffer[idx].tx + ft_char_buffer[idx].bw / ft_atlasWidth, 0};
        coords[n + 1] = point{x2,     -y2 - h, ft_char_buffer[idx].tx,                                          ft_char_buffer[idx].bh / ft_atlasHeight};
        coords[n + 3] = point{x2 + w, -y2    , ft_char_buffer[idx].tx + ft_char_buffer[idx].bw / ft_atlasWidth, 0};
        coords[n + 4] = point{x2,     -y2 - h, ft_char_buffer[idx].tx,                                          ft_char_buffer[idx].bh / ft_atlasHeight};
        coords[n + 5] = point{x2 + w, -y2 - h, ft_char_buffer[idx].tx + ft_char_buffer[idx].bw /ft_atlasWidth,  ft_char_buffer[idx].bh / ft_atlasHeight};

        n += 6;
    }

    /// upload the vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_DYNAMIC_DRAW);

    /// finally draw the text
    glDrawArrays(GL_TRIANGLES, 0, n);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(currentProgram);

    glDisable(GL_BLEND);
}

void
textRenderCleanup()
{
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);

    glDeleteProgram(fontProgram);
    glDeleteVertexArrays(1, &fontVAO);
    glDeleteBuffers(1 ,&fontVBO);
    glDeleteTextures(1, &fontTexID);
}
