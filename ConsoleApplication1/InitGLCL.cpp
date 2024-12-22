//#include <CL/cl.hpp>
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//// Inicjalizacja OpenGL
//GLuint vbo, nbo;
//void initOpenGL() {
//    glGenBuffers(1, &vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Punkt) * N_X * N_Y, NULL, GL_DYNAMIC_DRAW);
//
//    glGenBuffers(1, &nbo);
//    glBindBuffer(GL_ARRAY_BUFFER, nbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Normal) * N_X * N_Y, NULL, GL_DYNAMIC_DRAW);
//}
//
//// Inicjalizacja OpenCL
//cl::Context context;
//cl::CommandQueue queue;
//cl::Program program;
//cl::Kernel kernel;
//cl::Buffer aa_buf, bb_buf, cl_vbo, cl_nbo;
//
//
//
//void initOpenCL() {
//    std::vector<cl::Platform> platforms;
//    cl::Platform::get(&platforms);
//    cl::Platform platform = platforms.front();
//
//    std::vector<cl::Device> devices;
//    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
//    cl::Device device = devices.front();
//
//    context = cl::Context({ device });
//    queue = cl::CommandQueue(context, device);
//
//    std::ifstream sourceFile("kernel.cl");
//    std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
//    cl::Program::Sources sources(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
//
//    program = cl::Program(context, sources);
//    program.build({ device });
//
//    kernel = cl::Kernel(program, "obliczWspolrzedneINormalne");
//
//    aa_buf = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Punkt) * N_X * N_Y);
//    bb_buf = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Punkt) * N_X * N_Y);
//    cl_vbo = cl::BufferGL(context, CL_MEM_WRITE_ONLY, vbo);
//    cl_nbo = cl::BufferGL(context, CL_MEM_WRITE_ONLY, nbo);
//}