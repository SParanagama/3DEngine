#include "Texture.h"
#include "stb_image.h"

void Texture::LoadTexture(bool alpha) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);

    int32 width, height, nChannels;
    stbi_set_flip_vertically_on_load(true);
    uint8 *data = stbi_load(fileName, &width, &height, &nChannels, 0);
    if(data) {
        if(alpha)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        stbi_image_free(data);
        exit(-1);
    }
    stbi_image_free(data);
}

void Texture::BindTexture() {
    glBindTexture(GL_TEXTURE_2D, textureID);
}


