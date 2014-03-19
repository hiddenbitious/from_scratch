/**
 * Name:       TGA.cpp
 * Header:     tga.h
 * Purpose:    Load Compressed and Uncompressed TGA files
 * Functions:  LoadTGA(Texture * texture, char * filename)
 *             LoadCompressedTGA(Texture * texture, char * filename, FILE * fTGA)
 *             LoadUncompressedTGA(Texture * texture, char * filename, FILE * fTGA)
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#include "tga.h"

using namespace std;

C_TextureManager *C_TextureManager::classInstance = NULL;
/**
 * name:       LoadTGA(Texture * texture, char * filename)
 * function:   Open and test the file to make sure it is a valid TGA file
 * parems:     texture, pointer to a Texture structure
 *             filename, string pointing to file to open
 */

int
C_Texture::LoadTGA(const char *filename)          // Load a TGA file
{
   FILE * fTGA;                                    // File pointer to texture file
   fTGA = fopen(filename, "rb");                      // Open file for reading

   if(fTGA == NULL) {                              // If it didn't open....
      //MessageBox(NULL, "Could not open texture file", "ERROR", MB_OK);   // Display an error message
      return 0;                                          // Exit function
   }

   if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0) {             // Attempt to read 12 byte header from file
      //MessageBox(NULL, "Could not read file header", "ERROR", MB_OK);    // If it fails, display an error message
      if(fTGA != NULL) {                                    // Check to seeiffile is still open
         fclose(fTGA);                                      // If it is, close it
      }
      return 0;                                          // Exit function
   }

   if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0) {        // See if header matches the predefined header of
      // an Uncompressed TGA image
      LoadUncompressedTGA(filename, fTGA);                  // If so, jump to Uncompressed TGA loading code
   } else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0) { // See if header matches the predefined header of
      // an RLE compressed TGA image
      LoadCompressedTGA(filename, fTGA);                    // If so, jump to Compressed TGA loading code
   } else {                                              // If header matches neither type
      //MessageBox(NULL, "TGA file be type 2 or type 10 ", "Invalid Image", MB_OK); // Display an error
      fclose(fTGA);
      return 0;                                                // Exit function
   }
   return 1;                                             // All went well, continue on
}

int
C_Texture::LoadUncompressedTGA(const char *filename, FILE *fTGA)   // Load an uncompressed TGA (note, much of this code is based on NeHe's
{
   // TGA Loading code nehe.gamedev.net)
   if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {            // Read TGA header
      //MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);    // Display error
      if(fTGA != NULL) {                                    // if file is still open
         fclose(fTGA);                                      // Close it
      }
      return 0;                                          // Return failular
   }

   width  = tga.header[1] * 256 + tga.header[0];               // Determine The TGA Width (highbyte*256+lowbyte)
   height = tga.header[3] * 256 + tga.header[2];               // Determine The TGA Height   (highbyte*256+lowbyte)
   bpp   = tga.header[4];                             // Determine the bits per pixel
   tga.Width      = width;                            // Copy width into local structure
   tga.Height     = height;                              // Copy height into local structure
   tga.Bpp        = bpp;                                 // Copy BPP into local structure

   if((width <= 0) || (height <= 0) || ((bpp != 24) && (bpp != 32))) { // Make sure all information is valid
      //MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);   // Display Error
      if(fTGA != NULL) {                                    // Check if file is still open
         fclose(fTGA);                                      // If so, close it
      }
      return 0;                                          // Return failed
   }

   if(bpp == 24)                                      // If the BPP of the image is 24...
      type  = GL_RGB;                                 // Set Image type to GL_RGB
   else                                                  // Else if its 32 BPP
      type  = GL_RGBA;                                // Set image type to GL_RGBA

   tga.bytesPerPixel = (tga.Bpp / 8);                          // Compute the number of BYTES per pixel
   tga.imageSize     = (tga.bytesPerPixel * tga.Width * tga.Height);    // Compute the total amout ofmemory needed to store data
// imageData   = (GLubyte *)malloc(tga.imageSize);             // Allocate that much memory
   imageData   = new GLubyte[tga.imageSize];             // Allocate that much memory
   imageSize   = tga.imageSize;

   if(imageData == NULL) {                               // If no space was allocated
      //MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK); // Display Error
      fclose(fTGA);                                         // Close the file
      return 0;                                          // Return failed
   }

   if(fread(imageData, 1, tga.imageSize, fTGA) != tga.imageSize) { // Attempt to read image data
      //MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);     // Display Error
      if(imageData != NULL) {                            // If imagedata has data in it
         free(imageData);                             // Delete data from memory
      }
      fclose(fTGA);                                         // Close file
      return 0;                                          // Return failed
   }

   // Byte Swapping Optimized By Steve Thomas
   for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel) {
      imageData[cswap] ^= imageData[cswap + 2] ^=
                                      imageData[cswap] ^= imageData[cswap + 2];
   }

   fclose(fTGA);                                            // Close file
   return 1;                                             // Return success
}

int
C_Texture::LoadCompressedTGA(const char *filename, FILE * fTGA)    // Load COMPRESSED TGAs
{
   if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {            // Attempt to read header
      //MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);    // Display Error
      if(fTGA != NULL) {                                    // If file is open
         fclose(fTGA);                                      // Close it
      }
      return 0;                                          // Return failed
   }

   width  = tga.header[1] * 256 + tga.header[0];               // Determine The TGA Width (highbyte*256+lowbyte)
   height = tga.header[3] * 256 + tga.header[2];               // Determine The TGA Height   (highbyte*256+lowbyte)
   bpp   = tga.header[4];                             // Determine Bits Per Pixel
   tga.Width      = width;                            // Copy width to local structure
   tga.Height     = height;                              // Copy width to local structure
   tga.Bpp        = bpp;                                 // Copy width to local structure

   if((width <= 0) || (height <= 0) || ((bpp != 24) && (bpp != 32))) { //Make sure all texture info is ok
      //MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);   // If it isnt...Display error
      if(fTGA != NULL) {                                    // Check if file is open
         fclose(fTGA);                                      // Ifit is, close it
      }
      return 0;                                          // Return failed
   }

   if(bpp == 24)                                      // If the BPP of the image is 24...
      type  = GL_RGB;                                 // Set Image type to GL_RGB
   else                                                  // Else if its 32 BPP
      type  = GL_RGBA;                                // Set image type to GL_RGBA

   tga.bytesPerPixel = (tga.Bpp / 8);                          // Compute BYTES per pixel
   tga.imageSize     = (tga.bytesPerPixel * tga.Width * tga.Height);    // Compute amout of memory needed to store image
// imageData   = (GLubyte *)malloc(tga.imageSize);             // Allocate that much memory
   imageData   = new GLubyte[tga.imageSize];
   imageSize = tga.imageSize;
   if(imageData == NULL) {                               // If it wasnt allocated correctly..
      //MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK); // Display Error
      fclose(fTGA);                                         // Close file
      return 0;                                          // Return failed
   }

   GLuint pixelcount = tga.Height * tga.Width;                    // Nuber of pixels in the image
   GLuint currentpixel  = 0;                                   // Current pixel being read
   GLuint currentbyte   = 0;                                   // Current byte
   GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);        // Storage for 1 pixel

   do {
      GLubyte chunkheader = 0;                                 // Storage for "chunk" header

      if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0) {          // Read in the 1 byte header
         //MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);  // Display Error
         if(fTGA != NULL) {                                 // If file is open
            fclose(fTGA);                                   // Close file
         }
         if(imageData != NULL) {                         // If there is stored image data
            free(imageData);                          // Delete image data
         }
         return 0;                                       // Return failed
      }

      if(chunkheader < 128) {                                  // If the ehader is < 128, it means the that is the number of RAW color packets minus 1
         // that follow the header
         chunkheader++;                                     // add 1 to get number of following color values
         for(short counter = 0; counter < chunkheader; counter++) {  // Read RAW color values
            if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) { // Try to read 1 pixel
               //MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);     // IF we cant, display an error

               if(fTGA != NULL) {                                    // See if file is open
                  fclose(fTGA);                                      // If so, close file
               }

               if(colorbuffer != NULL) {                                // See if colorbuffer has data in it
                  free(colorbuffer);                                    // If so, delete it
               }

               if(imageData != NULL) {                            // See if there is stored Image data
                  free(imageData);                             // If so, delete it too
               }

               return 0;                                          // Return failed
            }
            // write to memory
            imageData[currentbyte      ] = colorbuffer[2];               // Flip R and B vcolor values around in the process
            imageData[currentbyte + 1  ] = colorbuffer[1];
            imageData[currentbyte + 2  ] = colorbuffer[0];

            if(tga.bytesPerPixel == 4) {                                // if its a 32 bpp image
               imageData[currentbyte + 3] = colorbuffer[3];          // copy the 4th byte
            }

            currentbyte += tga.bytesPerPixel;                              // Increase thecurrent byte by the number of bytes per pixel
            currentpixel++;                                             // Increase current pixel by 1

            if(currentpixel > pixelcount) {                             // Make sure we havent read too many pixels
               //MessageBox(NULL, "Too many pixels read", "ERROR", NULL);        // if there is too many... Display an error!

               if(fTGA != NULL) {                                    // If there is a file open
                  fclose(fTGA);                                      // Close file
               }

               if(colorbuffer != NULL) {                                // If there is data in colorbuffer
                  free(colorbuffer);                                    // Delete it
               }

               if(imageData != NULL) {                            // If there is Image data
                  free(imageData);                             // delete it
               }

               return 0;                                          // Return failed
            }
         }
      } else {                                                    // chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
         chunkheader -= 127;                                            // Subteact 127 to get rid of the ID bit
         if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) {  // Attempt to read following color values
            //MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);         // If attempt fails.. Display error (again)

            if(fTGA != NULL) {                                       // If thereis a file open
               fclose(fTGA);                                         // Close it
            }

            if(colorbuffer != NULL) {                                   // If there is data in the colorbuffer
               free(colorbuffer);                                       // delete it
            }

            if(imageData != NULL) {                               // If thereis image data
               free(imageData);                                // delete it
            }

            return 0;                                             // return failed
         }

         for(short counter = 0; counter < chunkheader; counter++) {           // copy the color into the image data as many times as dictated
            // by the header
            imageData[currentbyte      ] = colorbuffer[2];              // switch R and B bytes areound while copying
            imageData[currentbyte + 1  ] = colorbuffer[1];
            imageData[currentbyte + 2  ] = colorbuffer[0];

            if(tga.bytesPerPixel == 4) {                                // If TGA images is 32 bpp
               imageData[currentbyte + 3] = colorbuffer[3];          // Copy 4th byte
            }

            currentbyte += tga.bytesPerPixel;                              // Increase current byte by the number of bytes per pixel
            currentpixel++;                                             // Increase pixel count by 1

            if(currentpixel > pixelcount) {                             // Make sure we havent written too many pixels
               //MessageBox(NULL, "Too many pixels read", "ERROR", NULL);        // if there is too many... Display an error!

               if(fTGA != NULL) {                                    // If there is a file open
                  fclose(fTGA);                                      // Close file
               }

               if(colorbuffer != NULL) {                                // If there is data in colorbuffer
                  free(colorbuffer);                                    // Delete it
               }

               if(imageData != NULL) {                            // If there is Image data
                  free(imageData);                             // delete it
               }

               return 0;                                          // Return failed
            }
         }
      }
   }

   while(currentpixel < pixelcount);                                       // Loop while there are still pixels left
   fclose(fTGA);                                                     // Close the file
   return 1;                                                      // return success
}

C_TextureManager *C_TextureManager::getSingleton(void)
{
   if(!classInstance) {
      classInstance = new C_TextureManager();
   }

   return classInstance;
}

C_TextureManager::~C_TextureManager(void)
{
   for(unsigned int i = textures.size() - 1; i <= 0; ++i) {
      delete textures[i];
      textures.pop_back();
   }
}

C_Texture *
C_TextureManager::loadTexture(const char *filename)
{
   /// Search if texture with that filename already exists
   for(unsigned int i = 0; i < textures.size(); ++i) {
      if(!strcmp(filename, textures[i]->getTextureFilename())) {
         return textures[i]->refTexture();
      }
   }

   /// If it does not exist, create it, load it from disk and return it
   C_Texture *texture = new C_Texture;
   if(texture->loadGLTexture(filename))
      return texture;
   else {
      delete texture;
      return NULL;
   }
}

C_Texture::C_Texture(void)
{
   imageData = NULL;
   imageSize = 0;
   refCounter = 1;
   memset(filename, 0, FILENAME_LEN * sizeof(char));
}

C_Texture::~C_Texture()
{
   assert(!refCounter);

   if(imageData) {
      assert(imageSize);
      delete[] imageData;

      glDeleteTextures(1, &texID);
   }
}

C_Texture *
C_Texture::refTexture(void)
{
    ++refCounter;
    return this;
}

unsigned int
C_Texture::unrefTexture(void)
{
   return --refCounter;
}

C_Texture &C_Texture::operator= (const C_Texture &texture)
{
   assert(0 && "You are copying a texture... Are you sure?");

   if(this != &texture) {
      bpp = texture.bpp;
      width = texture.width;
      height = texture.height;
      texID = texture.texID;
      type = texture.type;
      imageData = NULL;
   }
   return *this;
}

bool
C_Texture::loadGLTexture(const char *filename)
{
   printf("-----------\n");
   printf("Loading texture: \"%s\"...", filename);

   if(!LoadTGA(filename)) {
      printf("Error loading texture %s.\n", filename);
      assert(0);
      return false;
   }

   glGenTextures(1, &texID);
   glBindTexture(GL_TEXTURE_2D, texID);
   glTexImage2D(GL_TEXTURE_2D, 0, bpp / 8, width, height, 0, type, GL_UNSIGNED_BYTE, imageData);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   memcpy(this->filename, filename, strlen(filename) * sizeof(char));

   printf(" done!\n");
   printf("\tbpp: %d\n", bpp);
   printf("\twidth: %d\n", width);
   printf("\theight: %d\n", height);
   printf("\ttexID: %d\n", texID);
   printf("\ttype: %d\n", type);
   printf("-----------\n");
   return true;
}
