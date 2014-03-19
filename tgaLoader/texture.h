#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <stdio.h>

class C_Texture
{
private:
   int LoadUncompressedTGA(const char *, FILE *);  // Load an Uncompressed file
   int LoadCompressedTGA(const char *, FILE *);    // Load a Compressed file
   int LoadTGA(const char *filename);

   unsigned char  *imageData;    /// Image Data (Up To 32 Bits)
   size_t         imageSize;     /// Size of image in bytes
   unsigned int   bpp;           /// Image Color Depth In Bits Per Pixel
   unsigned int   width;         /// Image Width
   unsigned int   height;        /// Image Height
   unsigned int   texID;         /// Texture ID Used To Select A Texture
   unsigned int   type;          /// Image Type (GL_RGB, GL_RGBA)
   unsigned int   refCounter;    /// Reference counter

public:
   C_Texture(void) { imageData = NULL; imageSize = 0; refCounter = 1; }
   ~C_Texture(void);
   C_Texture &operator= (const C_Texture &texture);
   C_Texture *refTexture(void);
   unsigned int unrefTexture(void);

   bool loadGLTexture(const char *filename);
   inline unsigned int getGLtextureID(void) { return texID; }
};

#endif
