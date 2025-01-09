// WindowsWave.h
#ifndef WINDOWSWAVE_H
#define WINDOWSWAVE_H

#include <string>
#include <windows.h>
#include <GL/glew.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class WindowsWave {
public:
    WindowsWave(int width, int height, const std::string& title);
    ~WindowsWave();
    void Run();

private:
    void InitOpenGL();
    void InitOpenCL();
    void OnRenderFrame();
    void OnUpdateFrame();
    void CompileShaders();
    bool RunKernel(bool kt);

    GLuint nbo;
    cl_command_queue queue;
    cl_kernel kernel, kernelTrujkatow, kernelPrzygotujTrojkaty;
    cl_mem aaBuf, bbBuf, clNbo, vertexBuffer;
    cl_context context;
    GLuint _vertexBufferObject, _vertexArrayObject, _shaderProgram, _vertexShader, _fragmentShader;
    size_t globalWorkSize[2];
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;

    struct Punkt {
        float m, v, x;
    };

    struct PunktNormal {
        float x, y, z, nx, ny, nz;
    };

    bool kt2 = true;
    int N_X = 100;
    int N_Y = 100;
    float dt = 0.01f, w = -2.0f, czas = 0.0f;
    float* vertices;
    Punkt* aa;
};

#endif // WINDOWSWAVE_H
