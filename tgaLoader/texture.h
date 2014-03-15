#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <stdio.h>

class C_Texture
{
public:
	unsigned char	*imageData;    // Image Data (Up To 32 Bits)
	unsigned int	bpp;				// Image Color Depth In Bits Per Pixel
	unsigned int	width;			// Image Width
	unsigned int	height;			// Image Height
	unsigned int	texID;			// Texture ID Used To Select A Texture
	unsigned int	type;				// Image Type (GL_RGB, GL_RGBA)

   C_Texture(void) {};
   ~C_Texture(void);
   bool loadGLTexture(const char *filename);
};

int LoadUncompressedTGA(C_Texture *, const char *, FILE *);	// Load an Uncompressed file
int LoadCompressedTGA(C_Texture *, const char *, FILE *);		// Load a Compressed file
int LoadTGA(C_Texture * texture, const char *filename);

#endif
