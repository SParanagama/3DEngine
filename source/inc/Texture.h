#ifndef INC_3DENGINE_TEXTURE_H
#define INC_3DENGINE_TEXTURE_H

#include <glad/glad.h>
#include "Types.h"

struct Texture {

    char* fileName;
    uint32 textureID = 0;
    int32 wrapMode_s = GL_REPEAT;
    int32 wrapMode_t = GL_REPEAT;
    int32 minFilter  = GL_LINEAR;
    int32 maxFilter  = GL_LINEAR;

    void LoadTexture(bool alpha);
    void BindTexture();
};


#endif //INC_3DENGINE_TEXTURE_H
