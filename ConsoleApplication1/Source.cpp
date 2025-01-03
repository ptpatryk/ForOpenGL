//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <CL/cl.h>
//#include <CL/cl_gl.h>
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <cmath>
//#include <GLFW/glfw3native.h>
//struct Punkt {
//    float m, v, x;
//};
//
//struct PunktNormal {
//    float x, y, z, nx, ny, nz;
//};
//
//class WindowsWave {
//public:
//    WindowsWave(int width, int height, const std::string& title);
//    ~WindowsWave();
//    void Run();
//
//private:
//    void InitOpenGL();
//    void InitOpenCL();
//    void OnRenderFrame();
//    void OnUpdateFrame();
//    void CompileShaders();
//    void RunKernel(bool kt);
//
//    int nbo;
//    cl_command_queue queue;
//    cl_kernel kernel, kernelTrujkatow, kernelPrzygotujTrojkaty;
//    cl_mem aaBuf, bbBuf, clNbo, vertexBuffer;
//    cl_context context;
//    int N_X = 100, N_Y = 100;
//    float dt = 0.01f, w = -2.0f, czas = 0.0f;
//    Punkt* aa;
//    float* vertices;
//    GLuint _vertexBufferObject, _vertexArrayObject, _shaderProgram, _vertexShader, _fragmentShader;
//    bool kt2 = true;
//    size_t globalWorkSize[2];
//    bool r = true;
//    GLFWwindow* window;
//};
//
//WindowsWave::WindowsWave(int width, int height, const std::string& title) {
//    // Initialize GLFW
//    if (!glfwInit()) {
//        std::cerr << "Failed to initialize GLFW" << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    // Create a windowed mode window and its OpenGL context
//    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
//    if (!window) {
//        glfwTerminate();
//        std::cerr << "Failed to create GLFW window" << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    // Make the window's context current
//    glfwMakeContextCurrent(window);
//
//    // Initialize GLEW
//    if (glewInit() != GLEW_OK) {
//        std::cerr << "Failed to initialize GLEW" << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    aa = new Punkt[N_X * N_Y];
//    for (int i = 0; i < N_X * N_Y; ++i) {
//        aa[i].m = 1.0f;
//    }
//
//    vertices = new float[N_X * N_Y * 18 * 2]; // Adjust size as needed
//    globalWorkSize[0] = N_X;
//    globalWorkSize[1] = N_Y;
//
//    InitOpenGL();
//    InitOpenCL();
//}
//
//WindowsWave::~WindowsWave() {
//    // Clean up OpenGL resources
//    glDeleteShader(_vertexShader);
//    glDeleteShader(_fragmentShader);
//    glDeleteProgram(_shaderProgram);
//    glDeleteBuffers(1, &_vertexBufferObject);
//    glDeleteVertexArrays(1, &_vertexArrayObject);
//
//    // Clean up OpenCL resources
//    clReleaseMemObject(aaBuf);
//    clReleaseMemObject(bbBuf);
//    clReleaseMemObject(clNbo);
//    clReleaseCommandQueue(queue);
//    clReleaseContext(context);
//
//    delete[] aa;
//    delete[] vertices;
//
//    // Terminate GLFW
//    glfwDestroyWindow(window);
//    glfwTerminate();
//}
//
//void WindowsWave::InitOpenGL() {
//    // Compile shaders
//    CompileShaders();
//
//    // Create and bind vertex array and buffer objects
//    glGenBuffers(1, &nbo);
//    glBindBuffer(GL_ARRAY_BUFFER, nbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * N_X * N_Y * 18 * 2, nullptr, GL_DYNAMIC_DRAW);
//
//    glGenVertexArrays(1, &_vertexArrayObject);
//    glBindVertexArray(_vertexArrayObject);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//}
//
//void WindowsWave::InitOpenCL() {
//    // Get OpenGL context
//    cl_context_properties properties[] = {
//        CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
//        CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)),
//        CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
//        0
//    };
//
//    // Create OpenCL context
//    context = clCreateContext(properties, 1, &device, nullptr, nullptr, &res);
//    queue = clCreateCommandQueueWithProperties(context, device, nullptr, &res);
//
//    // Create OpenCL buffers
//    aaBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(Punkt) * N_X * N_Y, aa, &res);
//    bbBuf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(Punkt) * N_X * N_Y, nullptr, &res);
//    clNbo = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(PunktNormal) * N_X * N_Y, nullptr, &res);
//    vertexBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, nbo, &res);
//
//    // Load and build OpenCL program
//    std::ifstream kernelFile("kernel.cl");
//    std::string kernelSource((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
//    const char* kernelSourceCStr = kernelSource.c_str();
//    cl_program program = clCreateProgramWithSource(context, 1, &kernelSourceCStr, nullptr, &res);
//    clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
//
//    // Create OpenCL kernels
//    kernel = clCreateKernel(program, "obliczWspolrzedne", &res);
//    kernelTrujkatow = clCreateKernel(program, "obliczNormalne", &res);
//    kernelPrzygotujTrojkaty = clCreateKernel(program, "przygotujTrojkaty", &res);
//}
//
//void WindowsWave::CompileShaders() {
//    // Load and compile vertex shader
//    std::ifstream vertexShaderFile("vertex_shader.glsl");
//    std::string vertexShaderSource((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
//    const char* vertexShaderSourceCStr = vertexShaderSource.c_str();
//    _vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(_vertexShader, 1, &vertexShaderSourceCStr, nullptr);
//    glCompileShader(_vertexShader);
//
//    // Load and compile fragment shader
//    std::ifstream fragmentShaderFile("fragment_shader.glsl");
//    std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragmentShaderFile)), std::istreambuf_iterator<char>());
//    const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();
//    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(_fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);
//    glCompileShader(_fragmentShader);
//
//    // Link shaders into a program
//    _shaderProgram = glCreateProgram();
//    glAttachShader(_shaderProgram, _vertexShader);
//    glAttachShader(_shaderProgram, _fragmentShader);
//    glLinkProgram(_shaderProgram);
//}
//
//void WindowsWave::OnRenderFrame() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glUseProgram(_shaderProgram);
//    glBindVertexArray(_vertexArrayObject);
//    glDrawArrays(GL_TRIANGLES, 0, N_X * N_Y * 36);
//
//    glfwSwapBuffers(window);
//}
//
//void WindowsWave::OnUpdateFrame() {
//    if (r) {
//        kt2 = RunKernel(kt2);
//        czas += dt;
//        r = false;
//    }
//}
//
//void WindowsWave::RunKernel(bool kt) {
//    cl_mem b1 = kt ? aaBuf : bbBuf;
//    cl_mem b2 = kt ? bbBuf : aaBuf;
//
//    clSetKernelArg(kernel, 0, sizeof(cl_mem), &b1);
//    clSetKernelArg(kernel, 1, sizeof(cl_mem), &b2);
//    clSetKernelArg(kernel, 2, sizeof(float), &dt);
//    clSetKernelArg(kernel, 3, sizeof(float), &w);
//    clSetKernelArg(kernel, 4, sizeof(int), &N_X);
//    clSetKernelArg(kernel, 5, sizeof(int), &N_Y);
//    clSetKernelArg(kernel, 6, sizeof(float), &sin(czas));
//    clSetKernelArg(kernel, 7, sizeof(float), &czas);
//
//    clSetKernelArg(kernelTrujkatow, 0, sizeof(cl_mem), &b2);
//    clSetKernelArg(kernelTrujkatow, 1, sizeof(cl_mem), &clNbo);
//    clSetKernelArg(kernelTrujkatow, 2, sizeof(int), &N_X);
//    clSetKernelArg(kernelTrujkatow, 3, sizeof(int), &N_Y);
//
//    clSetKernelArg(kernelPrzygotujTrojkaty, 0, sizeof(cl_mem), &clNbo);
//    clSetKernelArg(kernelPrzygotujTrojkaty, 1, sizeof(cl_mem), &vertexBuffer);
//    clSetKernelArg(kernelPrzygotujTrojkaty, 2, sizeof(int), &N_X);
//    clSetKernelArg(kernelPrzygotujTrojkaty, 3, sizeof(int), &N_Y);
//
//    clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
//    clEnqueueNDRangeKernel(queue, kernelTrujkatow, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
//    clEnqueueNDRangeKernel(queue, kernelPrzygotujTrojkaty, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);
//
//    clFinish(queue);
//    return !kt;
//}
//
//void WindowsWave::Run() {
//    while (!glfwWindowShouldClose(window)) {
//        OnUpdateFrame();
//        OnRenderFrame();
//        glfwPollEvents();
//    }
//}
//
//int main() {
//    WindowsWave wave(800, 600, "WindowsWave");
//    wave.Run();
//    return 0;
//}
