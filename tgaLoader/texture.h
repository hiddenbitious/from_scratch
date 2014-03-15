#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <stdio.h>

class C_Texture
{
private:
   int LoadUncompressedTGA(const char *, FILE *);  // Load an Uncompressed file
   int LoadCompressedTGA(const char *, FILE *);    // Load a Compressed file
   int LoadTGA(const char *filename);

   unsigned char  *imageData;    // Image Data (Up To 32 Bits)
   unsigned int   bpp;           // Image Color Depth In Bits Per Pixel
   unsigned int   width;         // Image Width
   unsigned int   height;        // Image Height
   unsigned int   texID;         // Texture ID Used To Select A Texture
   unsigned int   type;          // Image Type (GL_RGB, GL_RGBA)

public:
   C_Texture(void) { imageData = NULL; };
   ~C_Texture(void);
   bool loadGLTexture(const char *filename);
   inline unsigned int getGLtextureID(void) { return texID; }
};

#endif
