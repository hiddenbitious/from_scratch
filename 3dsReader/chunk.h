/****************************************
*     ***************************       *
*         Diplomatiki Ergasia:			*
*                                       *
*		  Meleti kai Ylopoiish			*
*		  Algorithmon Grafikon			*
*                                       *
*     ***************************       *
*                                       *
*			  Syggrafeas:               *
*                                       *
*		  Apostolou Panagiotis			*
*                                       *
*     ***************************       *
****************************************/

//>----- Entry point (Primary Chunk at the start of the file ----------------

#define           PRIMARY                 0x4D4D

//>----- Main Chunks --------------------------------------------------------

#define           EDIT3DS                 0x3D3D  // Start of our actual objects
#define           KEYF3DS                 0xB000  // Start of the keyframe information

//>----- General Chunks -----------------------------------------------------

#define           VERSION           0x0002
#define           MESH_VERSION      0x3D3E
#define           KFVERSION         0x0005
#define           COLOR_F           0x0010
#define           COLOR_24          0x0011
#define           LIN_COLOR_24      0x0012
#define           LIN_COLOR_F       0x0013
#define           INT_PERCENTAGE    0x0030
#define           FLOAT_PERC        0x0031
#define           MASTER_SCALE      0x0100
#define           IMAGE_FILE        0x1100
#define           AMBIENT_LIGHT     0X2100

//>----- Object Chunks -----------------------------------------------------

#define           NAMED_OBJECT      0x4000
#define           OBJ_MESH          0x4100
#define           MESH_VERTICES     0x4110
#define           VERTEX_FLAGS      0x4111
#define           MESH_FACES        0x4120
#define           MESH_MATER        0x4130
#define           MESH_TEX_VERT     0x4140
#define           MESH_XFMATRIX     0x4160
#define           MESH_COLOR_IND    0x4165
#define           MESH_TEX_INFO     0x4170
#define           HEIRARCHY         0x4F00

//>----- Material Chunks ---------------------------------------------------

#define           MATERIAL          0xAFFF
#define           MAT_NAME          0xA000
#define           MAT_AMBIENT       0xA010
#define           MAT_DIFFUSE       0xA020
#define           MAT_SPECULAR      0xA030
#define           MAT_SHININESS     0xA040
#define           MAT_SHININESS_STR 0xA041
#define           MAT_FALLOFF       0xA052
#define           MAT_EMISSIVE      0xA080
#define           MAT_SHADER        0xA100
#define           MAT_TEXMAP        0xA200
#define           MAT_TEXFLNM       0xA300
#define           OBJ_LIGHT         0x4600
#define           OBJ_CAMERA        0x4700

//>----- KeyFrames Chunks --------------------------------------------------

#define           ANIM_HEADER       0xB00A
#define           ANIM_OBJ          0xB002
#define           ANIM_NAME         0xB010
#define           ANIM_POS          0xB020
#define           ANIM_ROT          0xB021
#define           ANIM_SCALE        0xB022
