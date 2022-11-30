//
// Created by mateuszp on 11/18/22.
//

#ifndef GAME2D_TEXTURE_H
#define GAME2D_TEXTURE_H

#include <glad/glad.h>

#include "stb_image.h"

class Texture {
public:
    static unsigned int createTexture(const char* filepath);
};


#endif //GAME2D_TEXTURE_H
