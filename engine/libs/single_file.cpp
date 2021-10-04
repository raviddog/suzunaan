//  execution unit for all single file header libraries

#define ASSETSYS_IMPLEMENTATION
#define STRPOOL_IMPLEMENTATION
#include "assetsys.h"

#define INI_IMPLEMENTATION
#include "ini.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM 
//  jpeg and png support only, rest unneeded
#include "stb_image.h"
