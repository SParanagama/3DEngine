#include <cstdlib>
#include "Shader.h"
#include "glad/glad.h"

Shader::Shader(const char *vertexShader, const char *fragmentShader) {

    uint32 vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShader, nullptr);
    glCompileShader(vertex);

    GLint success;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if(!success) {
        exit(1);
    }

    fragment = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(fragment, 1, &fragmentShader, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if(!success) {
        exit(1);
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    GLint status;
    glGetProgramiv(ID, GL_LINK_STATUS, &status);
    if(!status) {
        exit(1);
    }

    glValidateProgram(ID);
    glGetProgramiv(ID, GL_VALIDATE_STATUS, &status);
    if(!status) {
        exit(1);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::UseShader() {
    glUseProgram(ID);
}

void Shader::SetBool(const char *name, bool value) {
    glUniform1i(glGetUniformLocation(ID, name), (int32)value);
}

void Shader::SetInt(const char *name, int32 value) {
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetFloat(const char *name, float value) {
    glUniform1f(glGetUniformLocation(ID, name), value);
}
