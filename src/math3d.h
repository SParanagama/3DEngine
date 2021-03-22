#ifndef MATH_H
#define MATH_H

#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

#include <glm/vec3.hpp>

struct Vector3f
{
    float x;
    float y;
    float z;

    Vector3f() {}
    Vector3f(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}
    Vector3f(const Vector3f& copy): x(copy.x), y(copy.y), z(copy.z) {}

    Vector3f operator- (const Vector3f& rv) {
        Vector3f ret;

        ret.x = rv.x - x;
        ret.y = rv.y - y;
        ret.z = rv.z - z;

        return ret;
    };

    Vector3f Normalize() {
        Vector3f ret = {};
        float length = sqrtf(x*x + y*y + z*z);
        if(length != 0) {
            ret.x = x/length;
            ret.y = y/length;
            ret.z = z/length;
        }
        return ret;
    };

    Vector3f CrossProduct(const Vector3f& rv) {
        Vector3f ret;
        ret.x = y*rv.z - rv.y*z;
        ret.y = x*rv.z - rv.x*z;
        ret.z = x*rv.y - rv.x*y;

        return ret;
    };

    void printValues()
    {
        printf("x: %f, y: %f, z: %f\n", x, y, z);
    };
};

struct Matrix4f
{
    float m[4][4];

    Matrix4f operator* (const Matrix4f& rm) {
        Matrix4f mat;
        for(int i=0; i<4; ++i) {
            for(int j=0; j<4; ++j) {
                mat.m[i][j] = m[i][0] * rm.m[0][j] +
                              m[i][1] * rm.m[1][j] +
                              m[i][2] * rm.m[2][j] +
                              m[i][3] * rm.m[3][j];
            }
        }

        return mat;    
    };
};

inline Matrix4f GetTranslationMatrix(float x, float y, float z)
{
    Matrix4f mat = {};
    mat.m[0][0] = 1.0f; mat.m[0][3] = x;
    mat.m[1][1] = 1.0f; mat.m[1][3] = y;
    mat.m[2][2] = 1.0f; mat.m[2][3] = z;
    mat.m[3][3] = 1.0f;

    return mat;
}

inline void InitIdentityMatrix4f(Matrix4f& mat)
{
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
}

inline Matrix4f GetRotationMatrix(float x, float y, float z)
{
    // TODO: Quaternions
    Matrix4f mat = {};
    InitIdentityMatrix4f(mat);

    return mat;
}

inline Matrix4f GetScaleMatrix(float x, float y, float z)
{
    Matrix4f mat = {};
    mat.m[0][0] = x;
    mat.m[1][1] = y;
    mat.m[2][2] = z;
    mat.m[3][3] = 1.0f;

    return mat;
}

inline Matrix4f GetPerspectiveProjectionMatrix(
                        float aspectRatio,
                        float fov,
                        float nearZ, float farZ)
{
    Matrix4f mat = {};
    float tanHalfFOV = tanf(fov/2.0f);
    float zRange  = nearZ-farZ;
    
    mat.m[0][0] = 1.0f / (aspectRatio * tanHalfFOV);
    mat.m[1][1] = 1.0f / tanHalfFOV;
    mat.m[2][2] = (-nearZ-farZ)/zRange;
    mat.m[2][3] = 2.0f*nearZ*farZ/zRange;
    mat.m[3][2] = 1.0f;

    return mat;
}

/*
inline Matrix4f GetCameraTransformationMatrix(CameraState& cameraState)
{
    Vector3f pos = cameraState.pos;
    Vector3f n = cameraState.target;
    Vector3f u = cameraState.up.CrossProduct(n).Normalize(); // Left hand coordinate system
    Vector3f v = u.CrossProduct(n).Normalize(); // Left hand coordination system!
    
    Matrix4f cameraTranslationMatrix = {};
    InitIdentityMatrix4f(cameraTranslationMatrix);
    
    cameraTranslationMatrix.m[0][3] = -pos.x;
    cameraTranslationMatrix.m[1][3] = -pos.y;
    cameraTranslationMatrix.m[2][3] = -pos.z;
    cameraTranslationMatrix.m[3][3] = 1.0f;

    Matrix4f camRotMat ={};
    camRotMat.m[0][0] = u.x;  camRotMat.m[0][1] = u.y;  camRotMat.m[0][2] = u.z;  camRotMat.m[0][3] = 0.0f;
    camRotMat.m[1][0] = v.x;  camRotMat.m[1][1] = v.y;  camRotMat.m[1][2] = v.z;  camRotMat.m[1][3] = 0.0f;
    camRotMat.m[2][0] = n.x;  camRotMat.m[2][1] = n.y;  camRotMat.m[2][2] = n.z;  camRotMat.m[2][3] = 0.0f;
    camRotMat.m[3][0] = 0.0f; camRotMat.m[3][1] = 0.0f; camRotMat.m[3][2] = 0.0f; camRotMat.m[3][3] = 1.0f; 
    
    return camRotMat * cameraTranslationMatrix;
}
*/

#endif