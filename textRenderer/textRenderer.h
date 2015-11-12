#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include "../globals.h"

//prototypes
void textRenderInit(std::string font_filename, int fontSize);
void textRenderCleanup();
void textRenderDrawText(std::string text, float x, float y, float sx, float sy, float textColor[4]);

#endif
