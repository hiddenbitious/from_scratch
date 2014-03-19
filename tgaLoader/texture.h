#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <stdio.h>
#include <vector>

#define FILENAME_LEN    128

class C_Texture
{
private:
   int LoadUncompressedTGA(const char *, FILE *);  // Load an Uncompressed file
   int LoadCompressedTGA(const char *, FILE *);    // Load a Compressed file
   int LoadTGA(const char *filename);

   unsigned char  *imageData;             /// Image Data (Up To 32 Bits)
   size_t         imageSize;              /// Size of image in bytes
   unsigned int   bpp;                    /// Image Color Depth In Bits Per Pixel
   unsigned int   width;                  /// Image Width
   unsigned int   height;                 /// Image Height
   unsigned int   texID;                  /// Texture ID Used To Select A Texture
   unsigned int   type;                   /// Image Type (GL_RGB, GL_RGBA)
   unsigned int   refCounter;             /// Reference counter
   char           filename[FILENAME_LEN]; /// Texture's filename on disk

public:
   C_Texture(void);
   ~C_Texture(void);
   C_Texture &operator= (const C_Texture &texture);
   C_Texture *refTexture(void);
   unsigned int unrefTexture(void);

   bool loadGLTexture(const char *filename);
   inline unsigned int getGLtextureID(void) { return texID; }
   inline char *getTextureFilename(void) { return filename; }
};


class C_TextureManager
{
private:
   std::vector<C_Texture *> textures;
   C_TextureManager(void) {};
   static C_TextureManager *classInstance;

public:
   C_Texture *loadTexture(const char *filename);
   ~C_TextureManager(void);
   static C_TextureManager *getSingleton(void);
};


#endif
