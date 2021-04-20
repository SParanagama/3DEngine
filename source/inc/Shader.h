#ifndef INC_3DENGINE_SHADER_H
#define INC_3DENGINE_SHADER_H

#include "Types.h"

class Shader {
public:
    uint32 ID;

    Shader(const char *vertexShader, const char *fragmentShader);

    void UseShader();
    void SetBool(const char *name, bool value);
    void SetInt(const char *name, int32 value);
    void SetFloat(const char *name, float value);
};


#endif //INC_3DENGINE_SHADER_H
