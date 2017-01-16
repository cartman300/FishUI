#define FISH_VMM_IMPL
#include "stdafx.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_MALLOC FGUI_ALLOC
#define STBI_FREE FGUI_FREE
#define STBI_REALLOC FGUI_REALLOC

//#define STBI_ONLY_PNG

#include "stb_image.h"