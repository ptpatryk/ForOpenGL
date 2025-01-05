// WindowsWave.cpp
#include "WindowsWave.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <chrono>
#include <thread>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

WindowsWave::WindowsWave(int width, int height, const std::string& title) {
    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindowsWaveClass";
    RegisterClass(&wc);

    // Create the window
    hwnd = CreateWindowEx(
        0,
        L"WindowsWaveClass",
        std::wstring(title.begin(), title.end()).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!hwnd) {
        std::cerr << "Failed to create window" << std::endl;
        exit(EXIT_FAILURE);
    }

    ShowWindow(hwnd, SW_SHOW);

    // Get the device context
    hdc = GetDC(hwnd);

    // Set the pixel format for the device context
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    // Create the OpenGL rendering context
    hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    aa = new Punkt[N_X * N_Y];
    for (int i = 0; i < N_X * N_Y; ++i) {
        aa[i].m = 1.0f;
        aa[i].v = 0.0f;
        aa[i].x = 0.0f;
    }

    vertices = new float[N_X * N_Y * 18 * 2];
    globalWorkSize[0] = N_X;
    globalWorkSize[1] = N_Y;

    InitOpenGL();
    InitOpenCL();
}

WindowsWave::~WindowsWave() {
    // Clean up OpenGL resources
    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);
    glDeleteProgram(_shaderProgram);
    glDeleteBuffers(1, &_vertexBufferObject);
    glDeleteVertexArrays(1, &_vertexArrayObject);

    // Clean up OpenCL resources
    clReleaseMemObject(aaBuf);
    clReleaseMemObject(bbBuf);
    clReleaseMemObject(clNbo);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    delete[] aa;
    delete[] vertices;

    // Clean up OpenGL context
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);

    // Destroy the window
    DestroyWindow(hwnd);
}

void WindowsWave::InitOpenGL() {
    // Compile shaders
    CompileShaders();

    // Create and bind vertex array and buffer objects
    glGenBuffers(1, &nbo);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * N_X * N_Y * 18 * 2, nullptr, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Ustawienia œwiate³
    glUseProgram(_shaderProgram);
    GLint lightPosLoc = glGetUniformLocation(_shaderProgram, "lightPos");
    GLint viewPosLoc = glGetUniformLocation(_shaderProgram, "viewPos");
    GLint lightColorLoc = glGetUniformLocation(_shaderProgram, "lightColor");
    GLint objectColorLoc = glGetUniformLocation(_shaderProgram, "objectColor");

    glUniform3f(lightPosLoc, 1.2f, 1.0f, 30.0f);
    glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);

    // Ustawienia macierzy
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationMatrix = rotationMatrixZ * rotationMatrixX;

    glm::mat4 projection = rotationMatrix * glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, -50.0f, 50.0f);

    GLint modelLoc = glGetUniformLocation(_shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(_shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(_shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


}

void WindowsWave::InitOpenCL() {
    // Get OpenGL context

    // Create OpenCL context
    cl_platform_id platform;
    cl_uint numPlatforms;
    cl_int res = clGetPlatformIDs(1, &platform, &numPlatforms);
    if (res != CL_SUCCESS) {
        std::cerr << "Failed to get OpenCL platform" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Get OpenCL device
    cl_device_id device;
    cl_uint numDevices;
    res = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &numDevices);
    if (res != CL_SUCCESS) {
        std::cerr << "Failed to get OpenCL device" << std::endl;
        exit(EXIT_FAILURE);
    }

    cl_context_properties properties[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
        0
    };

    context = clCreateContext(properties, 1, &device, nullptr, nullptr, &res);
    queue = clCreateCommandQueue(context, device, 0, &res);

    // Create OpenCL buffers
    aaBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(Punkt) * N_X * N_Y, aa, &res);
    bbBuf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(Punkt) * N_X * N_Y, nullptr, &res);
    clNbo = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(PunktNormal) * N_X * N_Y, nullptr, &res);
    vertexBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, nbo, &res);

    // Load and build OpenCL program
    std::ifstream kernelFile("kernel.cl");
    std::string kernelSource((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
    const char* kernelSourceCStr = kernelSource.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSourceCStr, nullptr, &res);
    clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);

    // Create OpenCL kernels
    kernel = clCreateKernel(program, "obliczWspolrzedne", &res);
    kernelTrujkatow = clCreateKernel(program, "obliczNormalne", &res);
    kernelPrzygotujTrojkaty = clCreateKernel(program, "przygotujTrojkaty", &res);
}

void WindowsWave::CompileShaders() {
    // Load and compile vertex shader
    std::ifstream vertexShaderFile("vertex_shader.glsl");
    std::string vertexShaderSource((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
    const char* vertexShaderSourceCStr = vertexShaderSource.c_str();
    _vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(_vertexShader, 1, &vertexShaderSourceCStr, nullptr);
    glCompileShader(_vertexShader);

    // Load and compile fragment shader
    std::ifstream fragmentShaderFile("fragment_shader.glsl");
    std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragmentShaderFile)), std::istreambuf_iterator<char>());
    const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();
    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(_fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);
    glCompileShader(_fragmentShader);

    // Link shaders into a program
    _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, _vertexShader);
    glAttachShader(_shaderProgram, _fragmentShader);
    glLinkProgram(_shaderProgram);
}

void WindowsWave::OnRenderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(_shaderProgram);
    glBindVertexArray(_vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, N_X * N_Y * 36);

    SwapBuffers(hdc);
}

void WindowsWave::OnUpdateFrame() {
    //if (r) {
        kt2 = RunKernel(kt2);
        czas += dt;
        r = false;
        OnRenderFrame();
    //}
}

bool WindowsWave::RunKernel(bool kt) {
    cl_mem b1 = kt ? aaBuf : bbBuf;
    cl_mem b2 = kt ? bbBuf : aaBuf;
    float mysin = sin(czas);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &b1);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &b2);
    clSetKernelArg(kernel, 2, sizeof(float), &dt);
    clSetKernelArg(kernel, 3, sizeof(float), &w);
    clSetKernelArg(kernel, 4, sizeof(int), &N_X);
    clSetKernelArg(kernel, 5, sizeof(int), &N_Y);
    clSetKernelArg(kernel, 6, sizeof(float), &mysin);
    clSetKernelArg(kernel, 7, sizeof(float), &czas);

    clSetKernelArg(kernelTrujkatow, 0, sizeof(cl_mem), &b2);
    clSetKernelArg(kernelTrujkatow, 1, sizeof(cl_mem), &clNbo);
    clSetKernelArg(kernelTrujkatow, 2, sizeof(int), &N_X);
    clSetKernelArg(kernelTrujkatow, 3, sizeof(int), &N_Y);

    clSetKernelArg(kernelPrzygotujTrojkaty, 0, sizeof(cl_mem), &clNbo);
    clSetKernelArg(kernelPrzygotujTrojkaty, 1, sizeof(cl_mem), &vertexBuffer);
    clSetKernelArg(kernelPrzygotujTrojkaty, 2, sizeof(int), &N_X);
    clSetKernelArg(kernelPrzygotujTrojkaty, 3, sizeof(int), &N_Y);

    clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
    clEnqueueNDRangeKernel(queue, kernelTrujkatow, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
    clEnqueueNDRangeKernel(queue, kernelPrzygotujTrojkaty, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);

    clFinish(queue);
    return !kt;
}

void WindowsWave::Run() {
    MSG msg = {};
    auto lastTime = std::chrono::high_resolution_clock::now();
    const std::chrono::milliseconds frameDuration(16); // 60 FPS

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);

            if (elapsedTime >= frameDuration) {
                OnUpdateFrame();
                OnRenderFrame();
                lastTime = currentTime;
            }
            else {
                std::this_thread::sleep_for(frameDuration - elapsedTime);
            }
        }
    }
}


/*
void WindowsWave::Run() {
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            OnUpdateFrame();
            OnRenderFrame();
        }
    }
}
*/