#include <iostream>
#include <string>
#include <cmath>
#include "assert.h"

#include <SDL.h>
#include <glad/glad.h>
#include <SDL_opengl.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <glm/gtx/rotate_vector.hpp>

//#include <Magick++.h>

#include "math3d.h"
#include "utils.h"

static int SCREEN_WIDTH = 1280;
static int SCREEN_HEIGHT = 720;

GLuint ibo;
GLuint vbo;
GLuint vao;
GLuint shaderProg;
GLuint uniformLocation;

GLuint textureObj;
GLuint samplerLocation;

struct InputState
{
    bool upPressed, downPressed, leftPressed, rightPressed;
    int mouseX, mouseY;
    int prevMouseX, prevMouseY;
    bool leftButtonPressed, rightButtonPressed;
};
static InputState inputState = {};

struct CameraState
{
    glm::vec3 pos;
    glm::vec3 up;
    glm::vec3 target;
};
static CameraState cameraState = {};

GLuint AddShader(const std::string& shader, GLenum shaderType)
{
    GLuint shaderObj = glCreateShader(shaderType);

    // Our shader is consisted of one file
    const GLchar* shaderSource = shader.c_str();
    glShaderSource(shaderObj, 1, &shaderSource, NULL);
    glCompileShader(shaderObj);

    GLint success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);

    if(!success) {
        GLchar info[1024];
        glGetShaderInfoLog(shaderObj, 1024, NULL, info);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", shaderType, info);
        exit(1);
    }

    glAttachShader(shaderProg, shaderObj);

    return shaderObj;
}

void LoadAndCompileShaders()
{
    std::string vs,fs;
    if(!ReadFile("shader.vs", vs)) {
        std::cout << "Error reading file" << std::endl;
        exit(1);
    }

    if(!ReadFile("shader.fs", fs)) {
        std::cout << "Error reading file" << std::endl;
        exit(1);
    }

    shaderProg = glCreateProgram();
    if(shaderProg == 0) {
        std::cout << "Error creating shader program" << std::endl;
        exit(1);
    }

    AddShader(vs, GL_VERTEX_SHADER);
    AddShader(fs, GL_FRAGMENT_SHADER);

    glLinkProgram(shaderProg);

    GLint status;
    glGetProgramiv(shaderProg, GL_LINK_STATUS, &status);
    if(!status) {
        GLchar errorLog[1024];
        glGetProgramInfoLog(shaderProg, 1024, NULL, errorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", errorLog);
        exit(1);
    }

    glValidateProgram(shaderProg);
    glGetProgramiv(shaderProg, GL_VALIDATE_STATUS, &status);
    if(!status) {
        GLchar errorLog[1024];
        glGetProgramInfoLog(shaderProg, 1024, NULL, errorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", errorLog);
        exit(1);
    }

    glUseProgram(shaderProg);

    uniformLocation = glGetUniformLocation(shaderProg, "worldMat");
    assert(uniformLocation != 0xFFFFFFFF);

    samplerLocation = glGetUniformLocation(shaderProg, "gSampler");
    assert(samplerLocation != 0xFFFFFFFF);
}

void PrepareAndBindVAO()
{
    glGenVertexArrays(1, &vao);

    // We store our drawing state in 'vao'
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    
    const unsigned int VERTEX_SIZE = 4*5;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (const GLvoid*)12);
    assert (glGetError() != GL_INVALID_OPERATION);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureObj);
    assert (glGetError() != GL_INVALID_OPERATION);

    // TODO: VAO state?
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert (glGetError() != GL_INVALID_OPERATION);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert (glGetError() != GL_INVALID_OPERATION);
    
    glBindVertexArray(0);
    //assert (glGetError() != GL_INVALID_OPERATION);
    printf("Unbound Vertex Array Object...\n");
}

void PrepareIndexBuffer()
{
    unsigned int indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 1, 2 };

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    assert (glGetError() != GL_INVALID_OPERATION);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    printf("Prepared Index Buffer...\n");
}

void PrepareVertexBuffers()
{
    // Vector3f vertexArr[4];
    // vertexArr[0] = Vector3f(-1.0f, -1.0f, 0.5773f);
    // vertexArr[1] = Vector3f(0.0f, -1.0f, -1.15475f);
    // vertexArr[2] = Vector3f(1.0f, -1.0f, 0.5773f);
    // vertexArr[3] = Vector3f(0.0f, 1.0f, 0.0f);

    float vertexArr[20] = {
        // Pos                   TexPos
       -1.0f, -1.0f, 0.5773f,    0.0f, 0.0f,
        0.0f, -1.0f, -1.15475f,  0.5f, 0.0f,
        1.0f, -1.0f, 0.5773f,    1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,       0.5f, 1.0f 
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArr), vertexArr, GL_STATIC_DRAW);
}

void Render()
{   
    //glEnable(GL_DEPTH_TEST); 
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    static float scale = 0.0f;
    scale += 1.0f;

    // Making things 3D
    float aspectRatio = SCREEN_WIDTH / (SCREEN_HEIGHT * 1.0f);
    float nearZ = 0.1f; float farZ = 50.0f;
    float fov = 45.0f;
    glm::mat4 projectionMat = glm::perspective(glm::radians(fov), aspectRatio, nearZ, farZ);
    
    // Making the vertices move
    glm::mat4 scaleMat = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 rotationMat = glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translationMat = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
    
    // * is left associative.
    // Therfore, translationMat will be multiplied by rotationMat and the result by scaleMat
    // Mt x Mr x Ms x V1
    // Mt x Mr x V2
    // Mt x V3 
    glm::mat4 modelMat = translationMat * rotationMat * scaleMat;
    printf("%f, %f, %f, %f\n%f, %f, %f, %f,\n%f, %f, %f, %f,\n%f, %f, %f, %f\n",
            modelMat[0][0], modelMat[0][1], modelMat[0][2], modelMat[0][3],
            modelMat[1][0], modelMat[1][1], modelMat[1][2], modelMat[1][3],
            modelMat[2][0], modelMat[2][1], modelMat[2][2], modelMat[2][3],
            modelMat[3][0], modelMat[3][1], modelMat[3][2], modelMat[3][3]
          );
    
    // Camera Tansformation
    glm::mat4 viewMat = glm::lookAt(cameraState.pos, cameraState.target, cameraState.up);

    // Perspective transformation
    glm::mat4 mvpMat = (projectionMat * viewMat * modelMat);
//    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mvpMat));
//    assert (glGetError() != GL_INVALID_OPERATION);
    
//    glUniform1i(samplerLocation, 0);

    // Before drawing we bind 'vao' which contains the vertex attribute and index buffer bindings
//    glBindVertexArray(vao);
//    assert (glGetError() != GL_INVALID_OPERATION);
//    printf("Bound Vertex Array Object...\n");

//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    assert (glGetError() != GL_INVALID_OPERATION);
//    printf("Bound Vertex Buffer...\n");

//    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
//    assert (glGetError() != GL_INVALID_OPERATION);
//    printf("Draw success...\n");

//    glBindVertexArray(0);
//    assert (glGetError() != GL_INVALID_OPERATION);
//    printf("Unbound Vertex Array Object...\n");
}

void GetInput()
{
    // Reset state
    inputState = {};

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W]) {
        inputState.upPressed = true;
        printf("Pressed W\n");
    }
    if (state[SDL_SCANCODE_S]) {
        inputState.downPressed = true;
        printf("Pressed S\n");
    }
    if (state[SDL_SCANCODE_A]) {
        inputState.leftPressed = true;
        printf("Pressed A\n");
    }
    if (state[SDL_SCANCODE_D]) {
        inputState.rightPressed = true;
        printf("Pressed D\n");
    }

    int x,y;
    int mouseState = SDL_GetMouseState(&x, &y);

    int mouseX = x - SCREEN_WIDTH/2;
    int mouseY = SCREEN_HEIGHT/2 - y;

    inputState.prevMouseX = inputState.mouseX;
    inputState.prevMouseY = inputState.mouseY; 
    inputState.mouseX = mouseX;
    inputState.mouseY = mouseY;
    printf("Mouse X: %i, Mouse Y: %i\n", mouseX, mouseY);

    if(mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        inputState.leftButtonPressed = true;
    }
    if(mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        inputState.rightButtonPressed = true;
    }

}

void Update()
{
    if(inputState.upPressed) {
        cameraState.pos.z += 0.01f;
    }
    if(inputState.downPressed) {
        cameraState.pos.z -= 0.01f;
    }
    if(inputState.leftPressed) {
        cameraState.pos.x -= 0.01f;
    }
    if(inputState.rightPressed) {
        cameraState.pos.x += 0.01f;
    }

    float rotationAngleAroundY = (inputState.mouseX - inputState.prevMouseX);
    float rotationAngleAroundX = (inputState.mouseY - inputState.prevMouseY);

    glm::vec3 rotAroundY = glm::rotateY(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(-inputState.mouseX*100.0f/(SCREEN_WIDTH*1.0f)));
    glm::vec3 target = glm::rotateX(rotAroundY, glm::radians(-inputState.mouseY*100.0f/(SCREEN_HEIGHT*1.0f)));

    printf("Target X: %.02f, Target Y: %.02f, Target Z: %.02f\n", target[0], target[1], target[2]);

    cameraState.target = target;
    cameraState.up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void LoadAssets()
{

}

void PrepareTextureObjects()
{
    glGenTextures(1, &textureObj);
    glBindTexture(GL_TEXTURE_2D, textureObj);

//    glTexImage2D(GL_TEXTURE_2D, 0,
//                 GL_RGBA, image->columns(), image->rows(), 0,
//                 GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
}

int main(int argc, char *argv[])
{
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        std::cout << "Error initializing SDL" << std::endl;
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8); 

    SDL_Window* window = SDL_CreateWindow("3DEngine", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    
    if(context == NULL) {
        std::cout << "Error creating Opengl context..\n";
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
        printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    }

    printf("GL version: %s\n", glGetString(GL_VERSION));
    
//    LoadAndCompileShaders();
    
//    LoadAssets();
//    PrepareIndexBuffer();
//    PrepareVertexBuffers();
//    PrepareTextureObjects();
//    PrepareAndBindVAO();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    cameraState.pos.z = -5.0f;

    // Start Event loop
    SDL_Event windowEvent;
    while(true)
    {
        if(SDL_PollEvent(&windowEvent))
        {
            if(windowEvent.type == SDL_QUIT) break;
        }

        GetInput();
        Update();
        Render();

        SDL_GL_SwapWindow(window);
    }

//    glDeleteProgram(shaderProg);
//    glDeleteBuffers(1, &vbo);
//    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}
